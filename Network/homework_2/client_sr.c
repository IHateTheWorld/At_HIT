#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#define SERVER_PORT 10000
#define SERVER_IP "127.0.0.1"
#define BUFFER_LENGTH 1026
#define RCV_WIND_SIZE 7
#define SEND_WIND_SIZE 7
#define SEQ_SIZE 20
#define DATA_SIZE 1024

int cfp, ack_send[SEQ_SIZE], rcv_base, store_base;
char store_buf[RCV_WIND_SIZE*DATA_SIZE];

void tips()
{
	printf("**********************************************\n");
	printf("|          -time to get current time         |\n");
	printf("|          -quit to exit client              |\n");
	printf("|          -testgbn [X] [Y] to test the gbn  |\n");
	printf("**********************************************\n");
}

int is_loss(float loss_ratio)
{
	int bound = (int)(loss_ratio*100);
	int r = rand()%101;
	if(r <= bound)
		return 1;
	return 0;
}

int main(void)
{
	cfp = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == cfp)
	{
		printf("create the client socket failed!\n");
		return -1;
	}
	printf("create the client socket succeeded!\n");

	struct sockaddr_in s_addr;
	int addr_len = sizeof(struct sockaddr_in);
	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	s_addr.sin_port = htons(SERVER_PORT);
	
	char buffer[BUFFER_LENGTH], cmd[128];
	float packet_lr = 0.2;
	float ack_lr = 0.2;
	FILE * wfp;
	if(!wfp)
		printf("opent the received file failed!\n");

	srand((unsigned int)time(NULL));

	while(1)
	{
		tips();
		
		gets(buffer);
		sscanf(buffer, "%s%f%f", cmd, &packet_lr, &ack_lr);
		
		if(!strcmp(cmd, "-testgbn"))
		{
			printf("Begin to test GBN protocol, please don't abort the process\n");
			printf("The loss ratio of packet is %.2f, the loss ratio of ack is %.2f\n", packet_lr, ack_lr);
			wfp = fopen("./file_rec.txt", "wb");
			int stage = 0, b, i; 
			unsigned char u_code;
			unsigned short seq;
			int recvsize, lseq, lsize, tmp;
			sendto(cfp, "-testgbn", strlen("-testgbn")+1, 0, (struct sockaddr *)&s_addr, addr_len);
			while(1)
			{
				recvsize = recvfrom(cfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&s_addr, &addr_len);
				printf("***************************** recvsize=%d\n", recvsize);
				if(recvsize < 0)
				{
					perror( "error in recvfrom: ");
					fclose(wfp);
					break;
				}
				//printf("%s\n", buffer);
				if(0 == stage) //shake hands
				{
					u_code = (unsigned char)buffer[0];
					if ((unsigned char)buffer[0] == 205)
					{
						printf("Ready for file transmission\n");
						buffer[0] = 200;
						buffer[1] = '\0';
						sendto(cfp, buffer, 2, 0, (struct sockaddr *)&s_addr, addr_len);
						sleep(0.2);
						rcv_base = 0; //init
						store_base = 0;
						lseq = 0;
						lsize = 0;
						memset(ack_send, 0, sizeof(ack_send));
						stage = 1;
					}
				}
				else if(1 == stage) //receive files
				{
					seq = (unsigned short)buffer[0];
					if(!((seq & 0xFF) ^ 0xFF)) //send the "got eof" signal
					{
						printf("has send...\n");
						sendto(cfp, "GOT EOF!", sizeof("GOT EOF!"), 0, (struct sockaddr *)&s_addr, addr_len);
						break;
					}
					else
					{
						b = is_loss(packet_lr);
						if(b)
						{
							printf("The packet with a seq of %d loss\n",seq);
							continue;
						}
						printf("recv a packet with a seq of %d\n",seq);
						printf("$$$$$$$$$$$$$$$$ seq: %d  store_base: %d  send_base: %d\n", seq, store_base, rcv_base);

						if((seq >= rcv_base && seq < rcv_base+RCV_WIND_SIZE) ||
								(seq < rcv_base && (seq-rcv_base+SEQ_SIZE) < RCV_WIND_SIZE)) //out of order
						{
							if(!ack_send[seq]) //ignore the repeat packet
							{
								if(seq >= rcv_base && seq < rcv_base+RCV_WIND_SIZE)
								{
									tmp = (store_base+(seq-rcv_base))%RCV_WIND_SIZE;
									memcpy(store_buf+DATA_SIZE*(tmp), &buffer[1], recvsize-2);
								}
								else
								{
									tmp = (store_base+(seq-rcv_base+SEQ_SIZE))%RCV_WIND_SIZE;
									memcpy(store_buf+DATA_SIZE*(tmp), &buffer[1], recvsize-2);
								}
								if((lseq <= seq && (seq-lseq) < RCV_WIND_SIZE) || 
										(lseq > seq && (lseq-seq+SEQ_SIZE) < RCV_WIND_SIZE))
								{
									lseq = seq;
									lsize = recvsize;
								}
								ack_send[seq] = 1;
								if(seq == rcv_base) //forward the window, write the buffer to the file
								{
									for(i = rcv_base+1; i < rcv_base+RCV_WIND_SIZE; ++i)
										if(!ack_send[i%SEQ_SIZE])
											break;
									if(lseq == (i-1)%SEQ_SIZE) //is just the rightest packet in the buffer
										recvsize = lsize;
									printf("************** lseq=%d, lsize=%d, i-1=%d\n", lseq, lsize, i-1);
									printf("************** write write ** %d %d %d\n", store_base, i-rcv_base, DATA_SIZE*(i-rcv_base-1)+recvsize-2);
									if(store_base + i - rcv_base > RCV_WIND_SIZE) //consider the store buffer is a circle
									{
										fwrite(store_buf+DATA_SIZE*store_base, 1, DATA_SIZE*(RCV_WIND_SIZE-store_base), wfp);
										fwrite(store_buf, 1, DATA_SIZE*(i-rcv_base-(RCV_WIND_SIZE-store_base)-1)+recvsize-2, wfp);
									}
									else
										fwrite(store_buf+DATA_SIZE*store_base, 1, DATA_SIZE*(i-rcv_base-1)+recvsize-2, wfp);
									int j;
									for(j = rcv_base; j < i; ++j) //don't forget that, update the window status flag
										ack_send[j%SEQ_SIZE] = 0;
									store_base += i - rcv_base;
									store_base %= RCV_WIND_SIZE;
									rcv_base = i%SEQ_SIZE;
								}
							}
							buffer[0] = seq;
							buffer[1] = '\0';
							b = is_loss(ack_lr);
							if(b)
							{
								printf("The ack of %d loss\n",(unsigned char)buffer[0]);
								continue;
							}
							sendto(cfp, buffer, 2, 0, (struct sockaddr*)&s_addr, addr_len);
							printf("send a ack of %d\n",(unsigned char)buffer[0]);
						}
						else
						//else if((seq < rcv_base && (rcv_base-seq) < SEND_WIND_SIZE) ||
						//		(seq > rcv_base && (rcv_base-seq+SEQ_SIZE) < RCV_WIND_SIZE)) //out of order
						{
							buffer[0] = seq;
							buffer[1] = '\0';
							b = is_loss(ack_lr);
							if(b)
							{
								printf("The ack of %d loss\n",(unsigned char)buffer[0]);
								continue;
							}
							sendto(cfp, buffer, 2, 0, (struct sockaddr*)&s_addr, addr_len);
							printf("send a ack of %d\n",(unsigned char)buffer[0]);
						}
						/*
						else
						{
							printf("ignore...\n");
						}
						*/
					}
					sleep(1);
				}
				else
					printf("wrong stage!\n");
			}
			fclose(wfp);
		}
		else
		{
			sendto(cfp, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&s_addr, addr_len);
			recvfrom(cfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&s_addr, &addr_len);
			printf("%s\n", buffer);
			if(!strcmp(buffer, "Good bye!"))
				break;
		}
	}

	close(cfp);

	return 0;
}

