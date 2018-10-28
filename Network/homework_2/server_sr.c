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

#define SERVER_PORT 10000
#define BUFFER_LENGTH 1026
#define SEND_WIND_SIZE 7
#define SEQ_SIZE 20
#define TIME_BOUND 16
#define DATA_SIZE 1024

int ack[SEQ_SIZE], times[SEQ_SIZE], send_base, next_seq;
int total_packet, sfp;
long file_len;

void gettime(char * ptime)
{
	char buffer[128];
	memset(buffer, 0, sizeof(buffer));
	time_t c_time;
	struct tm *p;
	time(&c_time);
	p = localtime(&c_time); 
	sprintf(buffer,"%d/%d/%d %d:%d:%d",
	p->tm_year + 1900,
	p->tm_mon + 1,
	p->tm_mday,
	p->tm_hour,
	p->tm_min,
	p->tm_sec);
	strcpy(ptime, buffer);
}

int is_available()
{
	int step;
	step = next_seq - send_base;
	step = step >= 0 ? step : step + SEQ_SIZE;

	if(step >= SEND_WIND_SIZE) //consider the condition of =
		return 0;
	if(ack[next_seq])
		return 1;

	return 0;
}

void send_a_packet(int curseq, int curpacket, char * buffer, char * send_buf, struct sockaddr_in c_addr)
{
	int addr_len = sizeof(struct sockaddr);
	int ret;

	printf(">>>>>> seq=%d packet=%d send_base=%d next_seq=%d\n", curseq, curpacket, send_base, next_seq);

	if(curpacket <= total_packet - 1)
	{
		buffer[0] = curseq;
		if(curpacket != total_packet - 1)
		{
			memcpy(&buffer[1], send_buf+DATA_SIZE*(curpacket%32), DATA_SIZE);
			buffer[DATA_SIZE+1] = '\0';
			//ret = sendto(sfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&c_addr, addr_len);
			ret = sendto(sfp, buffer, DATA_SIZE+2, 0, (struct sockaddr *)&c_addr, addr_len);
			//ret = sendto(sfp, buffer, BUFFER_LENGTH/2, 0, (struct sockaddr *)&c_addr, addr_len);
		}
		else
		{
			memcpy(&buffer[1], send_buf+DATA_SIZE*(curpacket%32), file_len-DATA_SIZE*curpacket);
			buffer[file_len-DATA_SIZE*curpacket+1] = '\0';
			ret = sendto(sfp, buffer, file_len-DATA_SIZE*curpacket+2, 0, (struct sockaddr *)&c_addr, addr_len);
			//ret = sendto(sfp, buffer, file_len-DATA_SIZE*curpacket+2, 0, (struct sockaddr *)&c_addr, addr_len);
		}
		ack[curseq] = 0;
		times[curseq] = 1;
		sleep(0.5);
	}
	else
		printf("curpacket has error!\n");
}

int main(void)
{
	int flags, on;
	sfp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //IPPROTO_UDP stand for UDP, also can use 0 in stead
	setsockopt(sfp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(-1 == sfp)
	{
		printf("create the server socket failed!\n");
		return -1;
	}
	printf("create the server socket succeeded!\n");
	flags = fcntl(sfp, F_GETFL, 0); //get the flag of the file
	if(-1 == flags)
	{
		printf("get the flags of the server socket failed!\n");
		return -1;
	}
	if(-1 == fcntl(sfp, F_SETFL, flags|O_NONBLOCK)) //add the nonblock flag to the file
	{
		printf("set the flags of the server socket failed!\n");
		return -1;
	}
	printf("set the flags of the server socket succeeded!\n");

	struct sockaddr_in s_addr;
	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons(SERVER_PORT);
	if(-1 == bind(sfp, (struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
		printf("bind the server socket failed!\n");
		return -1;
	}
	printf("bind the server socket succeeded!\n");

	struct sockaddr_in c_addr;
	int addr_len, recvsize;
	char buffer[BUFFER_LENGTH];
	addr_len = sizeof(struct sockaddr);

	while(1)
	{
		recvsize = recvfrom(sfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&c_addr, &addr_len);
		if(-1 == recvsize)
		{
			sleep(0.2);
			continue;
		}
		printf("Receive from client: %s\n", buffer);
		if(!strcmp(buffer, "-time"))
		{
			gettime(buffer);
			sendto(sfp, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&c_addr, addr_len);
		}
		else if(!strcmp(buffer, "-quit"))
		{
			strcpy(buffer, "Good bye!");
			sendto(sfp, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&c_addr, addr_len);
		}
		else if(strcmp(buffer, "-testgbn"))
			sendto(sfp, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&c_addr, addr_len);
		else
		{
			int status = 0, wait_time = 0, i;
			char sf_name[100];

			while(1)
			{
				if(!status) //shake hands
				{
					buffer[0] = 205;
					buffer[1] = '\0';
					sendto(sfp, buffer, 2, 0, (struct sockaddr *)&c_addr, addr_len);
					status = 1; //change the status so that to send file
					sleep(0.2);
					wait_time = 1;
				}
				else if(1 == status) //wait the 200 code
				{
					recvsize = recvfrom(sfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&c_addr, &addr_len);
					//sometime has little problem, should to drink some water
					/*
					recvsize = 1;
					buffer[0] = 200; //control just by hand 
					*/
					if(-1 == recvsize)
					{
						wait_time++;
						if(wait_time > TIME_BOUND)
						{
							printf("Wait 200 code timeout!\n");
							break;
						}
						printf("Sleeping... %d\n", wait_time);
						sleep(0.2);
						continue;
					}
					else if(200 == (unsigned char)buffer[0])
					{
						printf("Begin a file transfer\n");
						for(i = 0; i < SEQ_SIZE; ++i) //init
							ack[i] = 1;
						memset(times, 0, sizeof(times));
						send_base = 0;
						next_seq = 0;
						strcpy(sf_name, "./file_cont.txt");
						status = 2;
					}
					else
					{
						printf("Shake hands with the wrong code!\n");
						break;
					}
				}
				else if(2 == status) //send file
				{
					FILE * send_fp;
					char send_buf[DATA_SIZE*32]; //the double buffer
					long read_len;
					int le_ri, nextpacket, ack_no;
					send_fp = fopen(sf_name, "rb");
					if(!send_fp)
						printf("Open the file failed!\n");
					fseek(send_fp, 0, SEEK_END);
					file_len = ftell(send_fp); //get the length of the file
					fseek(send_fp, 0, SEEK_SET);
					total_packet = file_len / DATA_SIZE; //get the total packets of the file
					if(total_packet * DATA_SIZE < file_len)
						total_packet++;
					printf("File size is %ldB, each packet is %dB and packet total num is %d\n", file_len, DATA_SIZE, total_packet);
					le_ri = 0;
					nextpacket = 0; //the packet of the nextseq
					read_len = fread(send_buf+DATA_SIZE*16*le_ri, 1, DATA_SIZE*16, send_fp);
					printf("read_len: %ld\n", read_len);
					while(1) //that can be wrong...
					{
						if(is_available() && nextpacket < total_packet)
						{
							printf("Send a packet with a seq of %d %d\n", next_seq, nextpacket);
							send_a_packet(next_seq, nextpacket, buffer, send_buf, c_addr);
							next_seq++;
							next_seq %= SEQ_SIZE;
							nextpacket++;
							printf("================================================== next_packet=%d\n", nextpacket);
							if(nextpacket % 16 == 0 && nextpacket < total_packet)
							{
								le_ri ^= 1; //0->1 or 1->0
								read_len = fread(send_buf+DATA_SIZE*16*le_ri, 1, DATA_SIZE*16, send_fp); //read the half buffer
								printf("read_len: %ld\n", read_len);
							}
						}
						recvsize = recvfrom(sfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&c_addr, &addr_len);
						//;;;;
						if(-1 == recvsize)
						{
							if(next_seq > send_base)
							{
								for(i = send_base; i < next_seq; ++i)
								{
									times[i]++;
									if(times[i] > TIME_BOUND) //timeout
									{
										printf("Send the timeout packet: seq=%d, packet=%d\n", i, nextpacket-(next_seq-i));
										send_a_packet(i, nextpacket-(next_seq-i), buffer, send_buf, c_addr);
									}
								}
							}
							else if(next_seq < send_base)
							{
								for(i = send_base; i < SEQ_SIZE; ++i)
								{
									times[i]++;
									if(times[i] > TIME_BOUND) //timeout
									{
										printf("Send the timeout packet: seq=%d, packet=%d\n", i, nextpacket-(next_seq-i+SEQ_SIZE));
										send_a_packet(i, nextpacket-(next_seq-i+SEQ_SIZE), buffer, send_buf, c_addr);
									}
								}
								for(i = 0; i < next_seq; ++i)
								{
									times[i]++;
									if(times[i] > TIME_BOUND) //timeout
									{
										printf("Send the timeout packet: seq=%d, packet=%d\n", i, nextpacket-(next_seq-i));
										send_a_packet(i, nextpacket-(next_seq-i), buffer, send_buf, c_addr);
									}
								}
							}
						}
						//;;;;
						else //receive the ack
						{
							ack_no = buffer[0]; //that can be a repeat ack
							printf("Recv a ack of %d\n", ack_no);
							ack[ack_no] = 1;
							times[ack_no] = 0; //close the timer
							if(ack_no == send_base)
							{
								for(i = send_base+1; i < send_base+SEND_WIND_SIZE; ++i)
									if(ack[i%SEQ_SIZE] == 0 || i%SEQ_SIZE == next_seq)
										break;
								printf("Forward the send_window! %d->%d next_seq=%d packet:%d/%d\n", send_base, i, next_seq, nextpacket, total_packet);
								send_base = i%SEQ_SIZE; //forward the windows
								if(send_base == next_seq && nextpacket == total_packet) //succeeded to send the file!
								{
									printf("Recv the last packet!\n");
									buffer[0] = 0xFF;
									buffer[1] = '\0';
									sendto(sfp, buffer, 2, 0, (struct sockaddr *)&c_addr, addr_len);
									wait_time = 1; //start the timer to receive the 'get eof'
									status = 3;
									break;
								}
							}
						}
						sleep(1);
					}
					fclose(send_fp);
				}
				else if(3 == status)
				{
					recvsize = recvfrom(sfp, buffer, BUFFER_LENGTH, 0, (struct sockaddr *)&c_addr, &addr_len);
					if(-1 == recvsize)
					{
						wait_time++;
						if(!(wait_time % TIME_BOUND))
						{
							if(wait_time == 3*TIME_BOUND) //try 3 times
							{
								printf("receiver can't receive the succeeded signal!\n");
								break;
							}
							printf("send the succeeded signal again...\n");
							buffer[0] = 0xFF;
							buffer[1] = '\0';
							sendto(sfp, buffer, 2, 0, (struct sockaddr *)&c_addr, addr_len);
						}
						sleep(1);
						continue;
					}
					if(!strcmp(buffer, "GOT EOF!"))
					{
						printf("Send file succeeded!\n");
						break;
					}
				}
				else
				{
					printf("wrong status!\n");
					break;
				}
				sleep(0.5);
			}
		}
		
		//sleep(0.5); //control the timeout
	}

	close(sfp);

	return 0;
}

