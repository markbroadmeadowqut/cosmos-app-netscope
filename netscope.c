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
#include <arpa/inet.h>
#include <netinet/in.h> 
 
#define BUFLEN 512
#define PORT_LOCAL 	8887
#define PORT_REMOTE 8888
#define MAX_CLIENTS 1
#define NUM_PACKETS_TIMEOUT 100000
 
#define OCM_BASEADDR 			0xFFFC0000
#define OCM_MAPSIZE 			(64*1024)
//#define OCM_OFFSET_TIMESTAMP	0x00000FF8
//#define OCM_PAGE_INC			0x00001000				

#define REGS_BASEADDR 			0x80000000
#define REGS_MAPSIZE 			(4*1024)
#define REGS_OFFSET_TRIGGER		0x00000004

#define DMA_NUM_SIGNALS			32
//#define DMA_NUM_SIGNALS			128
#define DMA_BUFFER_LENGTH		0x10000 	// 64K = 1024*32*2
#define DMA_DATA_LENGTH			(2*DMA_NUM_SIGNALS)
#define DMA_OFFSET_TIMESTAMP	0x10

#define SG_NUM_PAGES		4
#define SG_NUM_SIGNALS		8
#define SG_OFFSET_START		0x04
#define SG_DATA_LENGTH		(SG_NUM_SIGNALS*2)	
#define SG_PAGE_LENGTH      (DMA_BUFFER_LENGTH/SG_NUM_PAGES)
#define SG_SAMPLES_PER_PAGE	(SG_PAGE_LENGTH/DMA_DATA_LENGTH)
#define SG_OFFSET_TIMESTAMP	(((SG_SAMPLES_PER_PAGE-1)*DMA_DATA_LENGTH)+DMA_OFFSET_TIMESTAMP)
 
int main(void) {
	int result;

	// socket descriptors
	int udps, tcps, client;
	struct sockaddr_in udpAddr, tcpAddr, clientAddr;
	int slen;

	// socket tx/rx buffers
	uint8_t txbuf[BUFLEN];
	uint8_t rxbuf[BUFLEN];

	// flow control management
	int bytesAvailable;
	uint32_t isAlive;

	uint8_t ch1, ch2;
	
	// OCM descriptors
	int fd;
	int runmap;
	uint8_t* pocm;
	uint8_t* pregs;

	uint16_t pPage;
	uint16_t pTimestamp;

	uint32_t timestamp;
	uint32_t prevTimestamp;
	
	int i, j;
	
	struct iovec* iov;
	
	struct msghdr msghdrs[SG_NUM_PAGES];
	struct iovec iovecs[SG_NUM_PAGES*SG_SAMPLES_PER_PAGE];
	
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

	for (i = 0; i < SG_NUM_PAGES; i++) {
		msghdrs[i].msg_name = &udpAddr;
		msghdrs[i].msg_namelen = sizeof(udpAddr);
		msghdrs[i].msg_iov = &iovecs[i*SG_SAMPLES_PER_PAGE];
		msghdrs[i].msg_iovlen = SG_SAMPLES_PER_PAGE;
		msghdrs[i].msg_control = NULL;
		msghdrs[i].msg_controllen = 0;
		msghdrs[i].msg_flags = 0;
		
		for (j = 0; j < SG_SAMPLES_PER_PAGE; j++) {
			iovecs[i*SG_SAMPLES_PER_PAGE + j].iov_base = &pocm[(i*SG_PAGE_LENGTH) + (j*DMA_DATA_LENGTH) + SG_OFFSET_START];
			iovecs[i*SG_SAMPLES_PER_PAGE + j].iov_len = SG_DATA_LENGTH;
		}
	}

/*	
	printf("pocm = %08X\r\n\r\n", pocm);

	for (i = 0; i < SG_NUM_PAGES; i++) {
		iov = msghdrs[i].msg_iov;
		
		for (j = 0; j < msghdrs[i].msg_iovlen; j++) {
			printf("msghdr[%d].msg_iov[%d].iov_base = %08X\r\n", i, j, iov[j].iov_base);
		}
		
		printf("\r\n");
	}
*/

	// map registers
	pregs = mmap(0, REGS_MAPSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, REGS_BASEADDR);
	if  (pregs == MAP_FAILED) {
		printf("ERROR: Failed to map REGS to memory.\n");
		return -1;
	}
	
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

	//Create UDP socket
	udps = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udps == -1) {
		printf("ERROR: Failed to create UDP socket.\n");
		return(0);
	}

	udpAddr.sin_family = AF_INET;
	udpAddr.sin_port = htons(PORT_REMOTE);

	slen = sizeof(struct sockaddr_in);

	while (1) {	
		//Wait for client connection
		client = accept(tcps, (struct sockaddr *) &clientAddr, (socklen_t*) &slen);
		printf("Netscope client connected.\n\r");

		//Set client address as destination for UDP datagrams
		udpAddr.sin_addr.s_addr = clientAddr.sin_addr.s_addr;

		isAlive = NUM_PACKETS_TIMEOUT;

		// reset sample pointers
		pPage = 0;
		//pTimestamp = OCM_OFFSET_TIMESTAMP;
		pTimestamp = SG_OFFSET_TIMESTAMP;
		
		timestamp = *(volatile uint32_t*)(pocm+pTimestamp);
		prevTimestamp = timestamp+1000000; 	// allow some time to sync
			
		while (isAlive) {
			
			timestamp = *(volatile uint32_t*)(pocm+pTimestamp);
			
			if (timestamp > prevTimestamp) {

				//sendto(udps, pocm+pPage, 64, MSG_CONFIRM, (const struct sockaddr *) &udpAddr, sizeof(udpAddr));
				//pTimestamp += OCM_PAGE_INC;
				//pPage += OCM_PAGE_INC;
				
				sendmsg(udps, &msghdrs[pPage&0x03], 0);
				pTimestamp += SG_PAGE_LENGTH;
				pPage++;
				

				ioctl(client, FIONREAD, &bytesAvailable);
				if (bytesAvailable > 0) {
					isAlive = NUM_PACKETS_TIMEOUT;
					read(client, rxbuf, BUFLEN);
				}
		
				prevTimestamp = timestamp;

				--isAlive;
			}

		}

		printf("Netscope client disconnected.\n\r");
		close(client);
	}

	return 0;
}
