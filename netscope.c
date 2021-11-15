#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <stdint.h>
#include <string.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <errno.h>
#include "smartdac_regs.h"
 
#define BUFLEN 512
#define PORT_LOCAL 	8887
#define MAX_CLIENTS 1
#define NUM_PACKETS_TIMEOUT 200 //This is not robust to sample rate variations
 
//Mapped memory definitions
//OCM (sampled values)
#define OCM_BASEADDR 			0xFFFC0000
#define OCM_MAPSIZE 			(64*1024)				
//AXI Registers
#define REGS_BASEADDR 			0x80000000
#define REGS_MAPSIZE 			(0x1D0050)

#define TCP_BUF_SIZE 4194304
#define TCP_MAX_XMIT TCP_BUF_SIZE

int main(void) {
	uint32_t SV_BUF_ADDRESS; 	// base address of buffer
	uint32_t SV_BUF_PAGELEN; 	// page size of SV buffer (in bytes)
	uint32_t SV_BUF_LENGTH;		// total length of buffer (in bytes)
	uint32_t SV_BUF_DATANUM;	// number of sampled value signals (excl. timestamp)
	uint32_t SV_BUF_TIMEOFF;	// offset of timestamp within page (in bytes)
	
	uint32_t SG_PAGE_NUM;		// number of SG pages into which the SV buffer is divided
	uint32_t SG_PAGE_LENGTH;	// length of the SG pages
	uint32_t SG_PAGE_NSAMPLES;	// number of samples per SG page
	
	uint64_t sv_mask;
	uint64_t sv_window_len;
	
	uint8_t xmit_inProgress = 0;
	uint8_t xmit_isPending = 0;
	uint32_t xmit_offset;
	uint32_t xmit_length;
	uint32_t xmit_maxBytes;

	int result;

	// socket descriptors
	int tcps, client;
	struct sockaddr_in tcpAddr, clientAddr;
	int slen;
	int nbytes;

	// socket tx/rx buffers
	//uint8_t txbuf[BUFLEN];
	uint8_t rxbuf[BUFLEN];

	// flow control management
	int bytesAvailable;
	uint32_t isAlive;

	
	// OCM descriptors
	int fd;
	uint8_t* pocm;
	uint8_t* pregs;

	uint16_t pPage;
	uint16_t pTimestamp;
	uint32_t sgPageCount;
	uint32_t tcpb_maxSgPages;
	uint32_t tcpb_bytesPerSample;
	uint32_t tcpb_bytesPerSgPage;

	uint64_t timestamp;
	uint64_t prevTimestamp;
	
	int i, j, k;
	
	//sv_mask analysis
	uint64_t sv_mask_z2o, sv_mask_o2z;
	uint8_t sv_mask_bi[32];
	uint8_t sv_mask_bl[32];
	uint8_t sv_mask_segs;
	uint8_t sv_mask_num;
	
	struct iovec iovecs[4*4096]; // max is SG_PAGE_LENGTH/4
	struct iovec* piovecs[4];
	
	uint8_t memory_buffer[TCP_BUF_SIZE];

	// open /dev/mem for mapping
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		printf("ERROR: Failed to open /dev/mem for mapping.\n");
		return -1;
	}

	// map OCM
	pocm = mmap(0, OCM_MAPSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, OCM_BASEADDR);
	if  (pocm == MAP_FAILED) {
		printf("ERROR: Failed to map OCM to memory.\n");
		return -1;
	}
	
	// map registers
	pregs = mmap(0, REGS_MAPSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, REGS_BASEADDR);
	if  (pregs == MAP_FAILED) {
		printf("ERROR: Failed to map REGS to memory.\n");
		return -1;
	}
	
	// open memory buffer
	//fd_buf = fmemopen(memory_buffer, 262144, "w");
	//if (fd_buf == NULL) {
	//	printf("Failed to open TCP buffer, errno = %d\n", errno);
	//}

	// get SV buffer parameters
	SV_BUF_ADDRESS 	= *(volatile uint32_t*)(pregs+NETSCOPEBA_OFFSET);
	SV_BUF_PAGELEN 	= *(volatile uint32_t*)(pregs+NETSCOPEPL_OFFSET);
	SV_BUF_LENGTH	= *(volatile uint32_t*)(pregs+NETSCOPEBL_OFFSET);
	SV_BUF_DATANUM	= *(volatile uint32_t*)(pregs+NETSCOPEDL_OFFSET);	// 4 words in timestamp
	SV_BUF_TIMEOFF	= *(volatile uint32_t*)(pregs+NETSCOPETO_OFFSET);
	printf("Loaded SV buffer details from PL:\n");
	printf("      Base address: 0x%08X\n", SV_BUF_ADDRESS);
	printf("     Buffer length: 0x%08X\n", SV_BUF_LENGTH);
	printf("       Page length: 0x%08X\n", SV_BUF_PAGELEN);
	printf("  Timestamp offset: 0x%08X\n", SV_BUF_TIMEOFF);
	printf("      Signal count: %d\n", SV_BUF_DATANUM);

	// set SG page parameters
	SG_PAGE_NUM = 4;
	SG_PAGE_LENGTH = SV_BUF_LENGTH/SG_PAGE_NUM;
	SG_PAGE_NSAMPLES = SG_PAGE_LENGTH/SV_BUF_PAGELEN;
	
	//Create TCP socket
	tcps = socket(AF_INET, SOCK_STREAM, 0);
	if (tcps == -1) {
		printf("ERROR: Failed to create TCP socket.\n");
		return(0);
	}

	memset(&tcpAddr, 0, sizeof(tcpAddr));

	tcpAddr.sin_family = AF_INET;
	tcpAddr.sin_addr.s_addr = INADDR_ANY;
	tcpAddr.sin_port = htons(PORT_LOCAL);

	result = bind(tcps, (struct sockaddr *) &tcpAddr, sizeof(tcpAddr));
	if (result < 0) {
		printf("ERROR: Failed to bind TCP socket to port %d.\n", PORT_LOCAL);
		return(0);	
	}

	listen(tcps, MAX_CLIENTS);

	slen = sizeof(struct sockaddr_in);

	while (1) {	
		//Wait for client connection
		client = accept(tcps, (struct sockaddr *) &clientAddr, (socklen_t*) &slen);
		printf("Netscope client connected.\n\r");

		printf("Waiting for 14 byte header from client...");
		do {
			ioctl(client, FIONREAD, &bytesAvailable);
		} while (bytesAvailable < 14);
		printf("DONE.\n\r");
		
		//Wait for header: 'NS' + sv_mask (64bit) + sv_window_len (32 bit)
		nbytes = read(client, rxbuf, 14);
		/*
		for (i = 0; i < 14; i++) {
			printf("[%02d] 0x%0X\n\r", i, rxbuf[i]);
		}
		*/
		if ((nbytes < 14) || (rxbuf[0] != 0x53) || (rxbuf[1] != 0x56)) {
			close(client);
			printf("Invalid header. Rejecting client.\n\r");
			continue;
		}


		printf("Received netscope header:\n");
		
		//Get SV mask
		sv_mask = rxbuf[9];
		sv_mask = (sv_mask<<8) | rxbuf[8];
		sv_mask = (sv_mask<<8) | rxbuf[7];
		sv_mask = (sv_mask<<8) | rxbuf[6];
		sv_mask = (sv_mask<<8) | rxbuf[5];
		sv_mask = (sv_mask<<8) | rxbuf[4];
		sv_mask = (sv_mask<<8) | rxbuf[3];
		sv_mask = (sv_mask<<8) | rxbuf[2];
		printf("  SV bitmask is 0x%llX.\n", sv_mask);
		
		//Get sample window length
		sv_window_len = rxbuf[13];
		sv_window_len = (sv_window_len<<8) | rxbuf[12];
		sv_window_len = (sv_window_len<<8) | rxbuf[11];
		sv_window_len = (sv_window_len<<8) | rxbuf[10];
		printf("  Window length is %lld.\n", sv_window_len);
		
		//Analyse sv_mask
		sv_mask_z2o = sv_mask&(sv_mask^(sv_mask<<1));		//zero-to-one transitions
		sv_mask_o2z = (~sv_mask)&(sv_mask^(sv_mask<<1));   	//one-to-zero transitions

		j = 0; //segment counter
		sv_mask_segs = 0;
		sv_mask_num = 0;
		for (i = 0; i < SV_BUF_DATANUM; i++) {
			if (sv_mask_z2o&((uint64_t)1<<i)) {
				//New segment
				sv_mask_bi[j] = i;	//Save start index
				sv_mask_bl[j] = 1;	//Set segment length to one
				sv_mask_segs++;		//Incrment segment count
			} else if (sv_mask_o2z&((uint64_t)1<<i)) {
				//End of segment
				sv_mask_num += sv_mask_bl[j]; //Increment signal count by segment length
				j++; //New segment
			} else {
				sv_mask_bl[j]++;
				if (i == (SV_BUF_DATANUM-1)) {
					if (sv_mask&((uint64_t)1<<i)) {
						//Last valid signal so finalise signal count
						sv_mask_num += sv_mask_bl[j];
					}
				}
			}
		}

		tcpb_bytesPerSample = 2*sv_mask_num;
		tcpb_bytesPerSgPage = tcpb_bytesPerSample*SG_PAGE_NSAMPLES;
		tcpb_maxSgPages = TCP_BUF_SIZE/tcpb_bytesPerSgPage;

		printf("Analysis of memory segments:\n\r");
		printf("  Segments: %d\n", sv_mask_segs);
		for (i = 0; i < sv_mask_segs; i++) {
			printf("  [%d] %d signal(s)\n", sv_mask_bi[i], sv_mask_bl[i]);
		}

		printf("  TCP bytes per sample:    %d\n", tcpb_bytesPerSample);
		printf("  TCP bytes per SG page:   %d\n", tcpb_bytesPerSgPage);
		printf("  TCP buffer max SG pages: %d\n", tcpb_maxSgPages);

		//Generate iovecs for all pages
		for (i = 0; i < SG_PAGE_NUM; i++) {
			piovecs[i] = iovecs+(i*SG_PAGE_NSAMPLES*sv_mask_segs);
			for (j = 0; j < SG_PAGE_NSAMPLES; j++) {
				for (k = 0; k < sv_mask_segs; k++) {
					iovecs[(i*SG_PAGE_NSAMPLES*sv_mask_segs) + (j*sv_mask_segs) + k].iov_base = &pocm[(i*SG_PAGE_LENGTH) + (j*SV_BUF_PAGELEN) + (2*sv_mask_bi[k])];
					iovecs[(i*SG_PAGE_NSAMPLES*sv_mask_segs) + (j*sv_mask_segs) + k].iov_len = 2*sv_mask_bl[k]; // 2 bytes per SV
				}
			}
		}



		isAlive = NUM_PACKETS_TIMEOUT;

		// reset sample pointers
		pPage = 0;
		pTimestamp = SV_BUF_TIMEOFF;
		sgPageCount = 0;
		
		/*
		j = tcpb_bytesPerSgPage*sgPageCount;
		for (i = 0; i < SG_PAGE_NSAMPLES*sv_mask_segs; i++) {
			printf("[%04d] Dest: 0x%08X, Src: 0x%08X, Len: 0x%02X\n", i, j, (uint32_t)piovecs[0][i].iov_base, piovecs[0][i].iov_len);
			j+=piovecs[0][i].iov_len;
		}
		*/

		xmit_length = 0;
		xmit_inProgress = 0;
		xmit_isPending = 0;

		timestamp = *(volatile uint64_t*)(pocm+pTimestamp);
		prevTimestamp = timestamp+0x1000000; 	// allow some time to sync, TODO: Fix this to be robust to sample rate
		pTimestamp += SG_PAGE_LENGTH;

		/*
		printf("First timestamp: 0x%016llX\n", timestamp);

		for (i = 0; i < 0x40; i+=4) {
			printf("[0x%02X] 0x%08X\n", i, *(volatile uint32_t*)(pocm+i));
		}
		*/

		while (isAlive) {
			
			timestamp = *(volatile uint64_t*)(pocm+pTimestamp);
			
			if (timestamp > prevTimestamp) {
				
				//printf("Timestamp: 0x%08X\n", timestamp);

				/*
				for (i = 0; i < SG_PAGE_NSAMPLES*sv_mask_segs; i++) {
					printf("[%02d] 0x%04X\n", i, *(uint16_t *) (piovecs[pPage&0x03][i].iov_base));
				}
				*/

				//nbytes = fseek(fd_buf,tcpb_bytesPerSgPage*sgPageCount,SEEK_SET);
				//printf("seek returned: %d, Error: %d\n", nbytes, errno);
				//nbytes = pwritev(fileno(fd_buf),piovecs[pPage&0x03],SG_PAGE_NSAMPLES*sv_mask_segs,tcpb_bytesPerSgPage*sgPageCount);
				//printf("pwritev Returned: %d, Error: %d\n", nbytes, errno);

				j = tcpb_bytesPerSgPage*sgPageCount;
				for (i = 0; i < SG_PAGE_NSAMPLES*sv_mask_segs; i++) {
					//printf("[%04d] Dest: 0x%08X, Src: 0x%08X, Len: 0x%02X\n", i, j, (uint32_t)piovecs[pPage&0x03][i].iov_base, piovecs[pPage&0x03][i].iov_len);
					memcpy(memory_buffer+j, piovecs[pPage&0x03][i].iov_base, piovecs[pPage&0x03][i].iov_len);
					//printf("src = %04X, dst = %04X\n", *(uint16_t *)(piovecs[pPage&0x03][i].iov_base+2), *(uint16_t *)(memory_buffer+j+2));
					j+=piovecs[pPage&0x03][i].iov_len;
				}
				//for (i = tcpb_bytesPerSgPage*sgPageCount; i < tcpb_bytesPerSgPage*(sgPageCount+1); i++) {
				//	printf("[%04X] %02X\n", i, memory_buffer[i]);
				//}


				pTimestamp += SG_PAGE_LENGTH;
				if (pTimestamp > SV_BUF_LENGTH) pTimestamp -= SV_BUF_LENGTH;
				pPage++;
				sgPageCount++;
				
				//Reset buffer offset if we will overflow on next write
				if (sgPageCount >= tcpb_maxSgPages) {
					sgPageCount = 0;
				}

				ioctl(client, FIONREAD, &bytesAvailable);
				if (bytesAvailable > 0) {
					isAlive = NUM_PACKETS_TIMEOUT;
					j = read(client, rxbuf, BUFLEN);
					for (i = 0; i < j; i++) {
						if (rxbuf[i]) {
							if (xmit_inProgress) {
								xmit_isPending = 1;
							} else {
								xmit_inProgress = 1;
								xmit_length = tcpb_bytesPerSample*sv_window_len;
								if (tcpb_bytesPerSgPage*sgPageCount < xmit_length) {
									xmit_offset = tcpb_bytesPerSgPage*(tcpb_maxSgPages+sgPageCount)-xmit_length;
								} else {
									xmit_offset = tcpb_bytesPerSgPage*sgPageCount - xmit_length;
								}
							}
							break;
						}
					}
				}
		
				prevTimestamp = timestamp;

				--isAlive;
			}

			if (xmit_length > 0) {

				if (xmit_offset+xmit_length > tcpb_maxSgPages*tcpb_bytesPerSgPage) {
					xmit_maxBytes = tcpb_maxSgPages*tcpb_bytesPerSgPage - xmit_offset;
				} else if (xmit_length > TCP_MAX_XMIT) {
					xmit_maxBytes = TCP_MAX_XMIT;
				} else {
					xmit_maxBytes = xmit_length;
				}
				nbytes = send(client, memory_buffer+xmit_offset, xmit_maxBytes, 0);
				//printf("XMIT: Sent %d bytes beginning offset 0x%X.\n", nbytes, xmit_offset);
				if (nbytes < 0) {
					//Error
					isAlive = 0;
				} else {
					xmit_offset += nbytes;
					xmit_length -= nbytes;
					if (!xmit_length) {
						if (xmit_isPending) {
							xmit_length = tcpb_bytesPerSample*sv_window_len;
							if (tcpb_bytesPerSgPage*sgPageCount < xmit_length) {
								xmit_offset = tcpb_bytesPerSgPage*(tcpb_maxSgPages+sgPageCount)-xmit_length;
							} else {
								xmit_offset = tcpb_bytesPerSgPage*sgPageCount - xmit_length;
							}
						} else {
							xmit_inProgress = 0;
						}
					} else {
						if (xmit_offset >= tcpb_maxSgPages*tcpb_bytesPerSgPage) {
							xmit_offset = 0;
						}
					}
				}
			}

		}

		printf("Netscope client disconnected.\n\r");
		close(client);
	}

	return 0;
}
