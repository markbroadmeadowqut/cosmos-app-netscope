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
#define KEEPALIVE_TIMEOUT 1000000000 //5s assuming 5ns clock
 
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
	
	uint32_t tcpb_unreadSamples = 0;
	uint32_t tcpb_pOldestUnreadSample = 0;

	//uint8_t xmit_inProgress = 0;
	//uint8_t xmit_isPending = 0;
	uint32_t xmit_status;
	uint32_t xmit_offset;
	uint32_t xmit_length;
	uint32_t xmit_maxBytes;
	//uint32_t xmit_holdoff = 0;

	uint64_t timeout;

	int result;

	// socket descriptors
	int tcps, client;
	struct sockaddr_in tcpAddr, clientAddr;
	int slen;
	int nbytes;

	// socket tx/rx buffers
	uint8_t rxbuf[BUFLEN];

	// flow control management
	int bytesAvailable;
	//uint32_t isAlive;
	
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

		//Wait for header from client
		//Timeout after 10 seconds
		printf("Waiting for 14 byte header from client...");
		for(i = 10; i; --i) {
			ioctl(client, FIONREAD, &bytesAvailable);
			if (bytesAvailable >= 14) {
				break;
			}
			sleep(1);
		}
		//If i=0 we timed out so reject client connection
		if (!i) {
			close(client);
			printf("TIMED OUT. Rejecting client.\n");
			continue;
		}
		//If we got here there are at least 14 bytes in buffer
		printf("DONE.\n\r");
		
		//Rx header: 'NS' + sv_mask (64bit) + sv_window_len (32 bit)
		nbytes = read(client, rxbuf, 14);

		//Check header validity
		if ((nbytes < 14) || (rxbuf[0] != 0x53) || (rxbuf[1] != 0x56)) {
			close(client);
			printf("Invalid header. Rejecting client.\n\r");
			continue;
		}

		//Print config details from header
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

		//This code determines the minimum number of contiguous segments of data
		//within a sample of signals. This is subsequently used to calculate
		//iovecs for vectored I/O.
		j = 0; //current segment index
		sv_mask_segs = 0; //number or segments
		sv_mask_num = 0; //number of enabled signals
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

		//Calculation of TCP-buffer-perspective lengths
		tcpb_bytesPerSample = 2*sv_mask_num;						//Bytes per sample
		tcpb_bytesPerSgPage = tcpb_bytesPerSample*SG_PAGE_NSAMPLES; //Bytes per page written into buffer
		tcpb_maxSgPages = TCP_BUF_SIZE/tcpb_bytesPerSgPage;			//Number of whole pages in buffer

		//Print segment analysis and TCP buffer parameters
		printf("Analysis of memory segments:\n\r");
		printf("  Segments: %d\n", sv_mask_segs);
		for (i = 0; i < sv_mask_segs; i++) {
			printf("  [%d] %d signal(s)\n", sv_mask_bi[i], sv_mask_bl[i]);
		}

		printf("  TCP bytes per sample:    %d\n", tcpb_bytesPerSample);
		printf("  TCP bytes per SG page:   %d\n", tcpb_bytesPerSgPage);
		printf("  TCP buffer max SG pages: %d\n", tcpb_maxSgPages);

		//Generate iovecs for all pages in OCM buffer
		for (i = 0; i < SG_PAGE_NUM; i++) {
			piovecs[i] = iovecs+(i*SG_PAGE_NSAMPLES*sv_mask_segs);
			for (j = 0; j < SG_PAGE_NSAMPLES; j++) {
				for (k = 0; k < sv_mask_segs; k++) {
					iovecs[(i*SG_PAGE_NSAMPLES*sv_mask_segs) + (j*sv_mask_segs) + k].iov_base = &pocm[(i*SG_PAGE_LENGTH) + (j*SV_BUF_PAGELEN) + (2*sv_mask_bi[k])];
					iovecs[(i*SG_PAGE_NSAMPLES*sv_mask_segs) + (j*sv_mask_segs) + k].iov_len = 2*sv_mask_bl[k]; // 2 bytes per SV
				}
			}
		}


		// reset sample pointers
		pPage = 0;
		pTimestamp = SV_BUF_TIMEOFF;
		sgPageCount = 0;
		tcpb_unreadSamples = 0;
		tcpb_pOldestUnreadSample = 0;

		xmit_status = 0;
		xmit_length = 0;

		timestamp = *(volatile uint64_t*)(pocm+pTimestamp);
		prevTimestamp = timestamp+0x1000000; 	// allow some time to sync, TODO: Fix this to be robust to sample rate
		pTimestamp += SG_PAGE_LENGTH;			// pTimestamp is advanced by one OCM page compared with pPage
		timeout = timestamp+KEEPALIVE_TIMEOUT;

		while (1) {
			
			//Get timestamp
			timestamp = *(volatile uint64_t*)(pocm+pTimestamp);
			
			//Wait for new page to be written to OCM
			// BEGIN NEW OCM PAGE
			if (timestamp > prevTimestamp) {

				//Copy OCM page into TCP buffer
				j = tcpb_bytesPerSgPage*sgPageCount;
				for (i = 0; i < SG_PAGE_NSAMPLES*sv_mask_segs; i++) {
					memcpy(memory_buffer+j, piovecs[pPage][i].iov_base, piovecs[pPage][i].iov_len);
					j+=piovecs[pPage][i].iov_len;
				}
				
				tcpb_unreadSamples += SG_PAGE_NSAMPLES; //Number of samples we just wrote into TCP buffer
				if (tcpb_unreadSamples > tcpb_maxSgPages*SG_PAGE_NSAMPLES) {
					// We overwrote some old data
					tcpb_pOldestUnreadSample += (tcpb_unreadSamples-(tcpb_maxSgPages*SG_PAGE_NSAMPLES));
					tcpb_pOldestUnreadSample = tcpb_pOldestUnreadSample%(tcpb_maxSgPages*SG_PAGE_NSAMPLES);
					tcpb_unreadSamples = tcpb_maxSgPages*SG_PAGE_NSAMPLES; //max buffered samples
				}

				//Advance timestamp and page pointers (OCM)
				pTimestamp += SG_PAGE_LENGTH;	//Advance timestamp pointer to next OCM page
				if (pTimestamp > SV_BUF_LENGTH) pTimestamp -= SV_BUF_LENGTH; //If OCM overflow, reset
				pPage = (pPage+1)%SG_PAGE_NUM;	//Advance OCM page counter

				//Advance page counter (TCP buffer)
				sgPageCount++;
				if (sgPageCount >= tcpb_maxSgPages) sgPageCount = 0; //If buffer overflow, reset
		
				prevTimestamp = timestamp;
			} // END NEW OCM PAGE

			//BEGIN TCP RX HANDLING
			ioctl(client, FIONREAD, &bytesAvailable);
			if (bytesAvailable > 0) {
				timeout = timestamp+KEEPALIVE_TIMEOUT; //Update timeout counter
				//Rx data
				nbytes = read(client, rxbuf, BUFLEN);
				for (i = 0; i < nbytes; i++) {
					//No further action for keepalive bytes (0x00)
					if (rxbuf[i]) {
						//Increment pending window count
						if (!(xmit_status++)) {
							//New xmit
							xmit_length = tcpb_bytesPerSample*sv_window_len;
							xmit_offset = tcpb_pOldestUnreadSample*tcpb_bytesPerSample;
						}
					}
				}
			} else if (timestamp > timeout) {
				printf("Client timed out. Disconnecting.\n");
				break;
			}
			//END TCP RX HANDLING

			//BEGIN TCP TX HANDLING
			if (xmit_status && tcpb_unreadSamples) {
				//We need to send data, and have data to send

				//Calculate maximum bytes for this xmit
				xmit_maxBytes = xmit_length;
				if (xmit_maxBytes > tcpb_unreadSamples*tcpb_bytesPerSample) xmit_maxBytes = tcpb_unreadSamples*tcpb_bytesPerSample; //Not all data available
				if (xmit_offset+xmit_maxBytes > tcpb_maxSgPages*tcpb_bytesPerSgPage) xmit_maxBytes = tcpb_maxSgPages*tcpb_bytesPerSgPage - xmit_offset; //Would overflow
				if (xmit_maxBytes > TCP_MAX_XMIT) xmit_maxBytes = TCP_MAX_XMIT; //More data than max send permitted

				//Xmit
				nbytes = send(client, memory_buffer+xmit_offset, xmit_maxBytes, 0);
				//printf("XMIT: Sent %d bytes beginning offset 0x%X.\n", nbytes, xmit_offset);

				if (nbytes < 0) {
					//Error
					printf("Error sending data to client. Disconnecting client.\n");
					break;
				} else {
					xmit_offset += nbytes;
					xmit_offset = xmit_offset%(tcpb_maxSgPages*tcpb_bytesPerSgPage); //overflow
					xmit_length -= nbytes;
					//TODO: These three line might be fragile?, could possibly split a sample
					tcpb_unreadSamples -= nbytes/tcpb_bytesPerSample;
					tcpb_pOldestUnreadSample += nbytes/tcpb_bytesPerSample;
					tcpb_pOldestUnreadSample = tcpb_pOldestUnreadSample%(tcpb_maxSgPages*SG_PAGE_NSAMPLES); //overflow
					//Completed window?
					if (!xmit_length) {
						if(--xmit_status) {
							//New xmit
							xmit_length = tcpb_bytesPerSample*sv_window_len;
							xmit_offset = tcpb_pOldestUnreadSample*tcpb_bytesPerSample;
						}
					}
				}
			} //END TCP TX HANDLING
		}

		printf("Netscope client disconnected.\n\r");
		close(client);
	}

	return 0;
}
