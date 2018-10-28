#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXSIZE 65507 //the max length of the packet
int file_id; //the name of the file received from host server
pthread_mutex_t fid_mutex; //the mutex of file_id

struct proxyparam
{
	int clientsocket;
	int serversocket;
};

struct httpheader
{
	char method[10];
	char url[1024];
	char host[1024];
	int port;
	char cookie[1024*10];
};

const int proxyport = 10240;

typedef struct 
{
	char url[1024];
	char modified_time[30];
	char file_name[20];
} a_cache;
#define MAXCACHE 200
#define MAXFH 100
a_cache caches[MAXCACHE];
int cache_num, fh_num, fish_num;
char fhs[MAXFH][1024];
char fishes[MAXFH][2][1024];

int read_init()
{
	FILE * cfp = fopen("cache_index.ci", "rb");
	if(!cfp)
	{
		printf("read: open the cache file failed!\n");
		return 0;
	}
	cache_num = 0;

	char url[1024], modified_time[50], file_name[20], host[1024];
	/*
	fscanf(cfp, "%s %s %s", url, file_name, modified_time);
	while(!feof(cfp))
	{
		strcpy(caches[cache_num].url, url);
		strcpy(caches[cache_num].modified_time, modified_time);
		strcpy(caches[cache_num].file_name, file_name);
		cache_num++;
		fscanf(cfp, "%s %s %s", url, modified_time, file_name);
	}
	
	file_id = cache_num; //the init of file_id;

	fclose(cfp);
	*/

	cfp = fopen("forbid_host.foh", "rb");
	if(!cfp)
	{
		printf("read: open the forbid_host file failed!\n");
		return 0;
	}
	fh_num = 0;
	fscanf(cfp, "%s", url);
	while(!feof(cfp))
	{
		strcpy(fhs[fh_num], url);
		fh_num++;
		fscanf(cfp, "%s", url);
	}
	fclose(cfp);
	
	cfp = fopen("fish_host.fih", "rb");
	if(!cfp)
	{
		printf("read: open the fish_host file failed!\n");
		return 0;
	}
	fish_num = 0;
	fscanf(cfp, "%s %s", host, url);
	while(!feof(cfp))
	{
		strcpy(fishes[fish_num][0], host);
		strcpy(fishes[fish_num][1], url);
		fish_num++;
		fscanf(cfp, "%s %s", host, url);
	}
	fclose(cfp);

	return 1;
}

int forbid_host(char * host)
{
	int i;

	for(i = 0; i < fh_num; ++i)
	{
		if(!strcmp(fhs[i], host))
			return 1;
	}
	return 0;
}

int fish_host(char * host)
{
	int i;

	for(i = 0; i < fish_num; ++i)
	{
		if(!strcmp(fishes[i][0], host))
			return i;
	}

	return -1;
}

int write_cache_file()
{
	FILE * cfp = fopen("cache_index.ci", "wb");
	if(!cfp)
	{
		printf("write: open the cache file failed!\n");
		return 0;
	}
	
	int i;
	for(i = 0; i < cache_num; ++i)
	{
		fprintf(cfp, "%s %s %s\n", caches[i].url, caches[i].file_name, caches[i].modified_time);
	}

	fclose(cfp);
	
	return 1;
}

int is_cache(char * url)
{
	int i;

	for(i = 0; i < cache_num; ++i)
	{
		if(!strcmp(caches[i].url, url))
			return i;
	}

	return -1;
}

int Http_status_code(char * buffer)
{
	int code = 0;
	char *p, cch[4];

	p = buffer;

	while(*p != '\r') //\r\n
	{
		if(*p == ' ')
		{
			p++;
			memcpy(cch, p, 3);
			cch[3] = '\0';
			code = atoi(cch);
			break;
		}
		p++;
	}

	return code;
}

int deal_cache(struct proxyparam pp, struct httpheader nh, char * buffer, int * bsize)
{
	int fid = is_cache(nh.url);
	if(-1 == fid)
		return 1;

	//insert or change the flag If-Modified-Since
	char *p, tmp[51], *gen_qst;
	int flag = 0;
	p = buffer;
	while(p)
	{
		if(*p == '\r') //\r\n
		{
			p += 2;
			if(*p == 'I')
			{
				memcpy(tmp, p, 17);
				tmp[17] = '\0';
				if(!strcmp(tmp, "If-Modified-Since")) //should change the flag
				{
					p += 2; //skip the ": "
					memcpy(p, caches[fid].modified_time, 29); //29 is the length of the GMT
					flag = 1;
					break;
				}
			}
			else if(*p == '\r')
			{
				memcpy(tmp, p-2, 4);
				tmp[4] = '\0';
				if(!strcmp(tmp, "\r\n\r\n"))
				{
					p -= 2;
					break;
				}
			}
		}
		p++;
	}
	if(flag)
		send(pp.serversocket, buffer, *bsize, 0);
	else
	{
		sprintf(tmp, "\r\nIf-Modified-Since: %s", caches[fid].modified_time); //stack attention: the length of tmp at least 51
		gen_qst = (char *)malloc(*bsize+strlen(tmp));
		memcpy(gen_qst, buffer, p-buffer);
		memcpy(gen_qst+(p-buffer), tmp, strlen(tmp));
		memcpy(gen_qst+(p-buffer)+strlen(tmp), p, *bsize-(p-buffer));
		send(pp.serversocket, gen_qst, *bsize+strlen(tmp), 0);
		free(gen_qst);
	}

	*bsize = recv(pp.serversocket, buffer, MAXSIZE, 0);
	if(*bsize <= 0)
	{
		printf("ignore the cache in proxy server!\n");
		return 1;
	}
	
    int hsc = Http_status_code(buffer);
	if(304 == hsc)
	{
		FILE * rfp;
		char send_buf[MAXSIZE];
		int read_bn;
		memset(send_buf, 0, sizeof(send_buf));
		rfp = fopen(caches[fid].file_name, "rb");
		read_bn = fread(send_buf, 1, MAXSIZE, rfp); //read_bit = read_bn * size(2th parament)
		while(read_bn)
		{
			send(pp.clientsocket, send_buf, read_bn, 0);
			read_bn = fread(send_buf, 1, MAXSIZE, rfp);
		}

		fclose(rfp);
		return 0;
	}
	else if(200 == hsc)
	{
		return 2;
	}
	else
		return 3;
}

int fetch_time(char * time, char * header)
{
	char * copy, * line;
	char tmp[20];
	
	copy = (char *)malloc(strlen(header)+1);
	memcpy(copy, header, strlen(header));
	copy[strlen(header)] = '\0';
	
	line = strtok(copy, "\r\n");
	while(line)
	{
		switch(line[0])
		{
			case 'L':
				memcpy(tmp, line, 13);
				tmp[13] = '\0';
				if(!strcmp(tmp, "Last-Modified"))
				{
					memcpy(time, &line[15], strlen(line)-15);
					time[strlen(line)-15] = '\0';
					free(copy);
					return 1;
				}
				break;
			default:
				break;
		}

		line = strtok(NULL, "\r\n");
	}
	free(copy);

	return 0;
}

int proxythread(int * accept_fp)
{
	char buffer[MAXSIZE], *copy_info;
	int recvsize;
	struct proxyparam pp; //the thread use the same memory
	pp.clientsocket = *accept_fp;
	pp.serversocket = -1;
	memset(buffer, 0, sizeof(buffer));
	recvsize = recv(pp.clientsocket, buffer, MAXSIZE, 0);
	if(recvsize <= 0)
	{
		printf("nothing send from client, quit!\n");
		sleep(0.2);
		close(pp.clientsocket);
		return -1;
	}
	copy_info = (char *)malloc(recvsize+1);
	memset(copy_info, 0, sizeof(copy_info));
	memcpy(copy_info, buffer, recvsize);

	struct httpheader nh;
	char * line, * cch, tmp[20];
	const char * delim = "\r\n";
	int i = 0, j = -1;
	memset(&nh, 0, sizeof(nh));
	line = strtok(copy_info, delim);
	for(cch = line; *cch != ' '; ++cch)
	{
		nh.method[i++] = *cch;
	}
	i = 0;
	for(cch++; *cch != ' '; ++cch)
	{
		nh.url[i++] = *cch;
		if(*cch == ':')
			j = i;
	}
	if(j >= 0 && nh.url[j] >= '0' && nh.url[j] <= '9')
	{
		if(nh.url[j] > '0' && (!atoi(&nh.url[j])))
			nh.port = 80;
		else if(nh.url[j] == '0' && strlen(&nh.url[j]) > 1)
			nh.port = 80;
		else
			nh.port = atoi(&nh.url[j]);
	}
	else
		nh.port = 80;
	line = strtok(NULL, delim);
	while(line)
	{
		switch(line[0])
		{
			case 'H':
				memcpy(tmp, line, 4);
				tmp[4] = '\0';
				if(!strcmp(tmp, "Host"))
					memcpy(nh.host, &line[6], strlen(line)-6);
				break;
			case 'C':
				memcpy(tmp, line, 6);
				tmp[6] = '\0';
				if(!strcmp(tmp, "Cookie"))
					memcpy(nh.cookie, &line[8], strlen(line)-8);
				break;
			default:
				break;
		}
		line = strtok(NULL, delim);
	}
	free(copy_info);

	//filt the host
	if(forbid_host(nh.host))
	{
		printf("the host server is forbided!\n");
		return 2;
	}
	//fish the host
	int fihn = fish_host(nh.host);
	if(fihn >= 0)
	{
		printf("fishing: %s ==> %s\n", nh.host, fishes[fihn][1]);
		strcpy(nh.host, fishes[fihn][1]);
		sprintf(buffer, "GET http://%s/ HTTP/1.1\r\nHost: %s\r\n\r\n", nh.host, nh.host);
	}

	struct sockaddr_in host_addr;
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(nh.port);
	struct hostent * host_info;
	host_info = gethostbyname(nh.host);	
	if(host_info == NULL)
	{
		printf("gethostbyname has error!\n");
		sleep(0.2);
		close(pp.clientsocket);
		pp.clientsocket = -1;
		return -1;
	}
	struct in_addr inaddr = *((struct in_addr*)*host_info->h_addr_list);
	host_addr.sin_addr.s_addr = inet_addr(inet_ntoa(inaddr));
	pp.serversocket = socket(AF_INET, SOCK_STREAM, 0);
	int on;
	setsockopt(pp.serversocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(-1 == pp.serversocket)
	{
		printf("create the socket that connecting the host server failed!\n");
		sleep(0.2);
		close(pp.clientsocket);
		pp.clientsocket = -1;
		return -1;
	}
	printf("create the socket that connecting the host server succeeded!\n");
		
	if(-1 == connect(pp.serversocket, (struct sockaddr *)&host_addr, sizeof(host_addr)))
	{
		printf("connect to the host server failed!\n");
		sleep(0.2);
		close(pp.clientsocket);
		pp.clientsocket = -1;
		close(pp.serversocket);
		pp.serversocket = -1;
		return -1;
	}
	printf("connect to the host server succeeded!\n");

	//deal with cache
	int ret;
	ret = deal_cache(pp, nh, buffer, &recvsize);
	if(0 == ret)
	{
		printf("%s deal with cache succeeded!\n", nh.url);
		sleep(0.2);
		close(pp.clientsocket);
		pp.clientsocket = -1;
		close(pp.serversocket);
		pp.serversocket = -1;
		return 1;
	}
	else if(1 == ret)
	{
		ret = send(pp.serversocket, buffer, recvsize, 0);

		//attention: maybe split
		recvsize = recv(pp.serversocket, buffer, MAXSIZE, 0);
		if(recvsize <= 0)
		{
			sleep(0.2);
			close(pp.clientsocket);
			pp.clientsocket = -1;
			close(pp.serversocket);
			pp.serversocket = -1;
			return -1;
		}
	}
	char file_name[20], url_info[1024];
	char time[1024];
	int hsc;
	int tp;
	int content_len=-1, cur_len, data_len;
	char *cp, *header;
	cp = buffer;
	while(*cp)
	{
		if(*cp == '\r')
		{
			memcpy(tmp, cp, 4);
			tmp[4] = '\0';
			if(!strcmp(tmp, "\r\n\r\n"))
			{
				break;
			}
		}
		cp++;
	}
	cur_len = cp-buffer;
	header = (char *)malloc(cur_len+1);
	memset(header, 0, cur_len+1);
	memcpy(header, buffer, cur_len);
	header[cur_len] = '\0';
	hsc = Http_status_code(buffer); //the http state code
	tp = fetch_time(time, header);
	FILE * store_fp;
	if(200 == hsc && tp)
	{
		pthread_mutex_lock(&fid_mutex);
		sprintf(file_name, "%d.tmp", file_id%MAXCACHE);
		strcpy(caches[file_id].url, nh.url);
		strcpy(caches[file_id].modified_time, time);
		strcpy(caches[file_id].file_name, file_name);
		file_id++;
		if(MAXCACHE != cache_num)
			cache_num++;
		write_cache_file(); //there should do better...
		pthread_mutex_unlock(&fid_mutex);
		store_fp = fopen(file_name, "wb");
		if(!store_fp)
			printf("open the file store cache information failed!\n");
		else
			fwrite(buffer, recvsize, 1, store_fp);
	}
	ret = send(pp.clientsocket, buffer, recvsize, 0);

	data_len = recvsize - cur_len - 4;
	line = strtok(header, delim);
	while(line)
	{
		if(line[0] == 'C')
		{
			memcpy(tmp, line, 14);
			tmp[14] = '\0';
			if(!strcmp(tmp, "Content-Length"))
			{
				content_len = atoi(&line[16]);
				break;
			}
		}
		line = strtok(NULL, delim);
	}
	free(header);
	if(-1 == content_len)
	{
		content_len = data_len;
	}
	while(data_len < content_len)
	{
		recvsize = recv(pp.serversocket, buffer, MAXSIZE, 0);
		if(recvsize <= 0)
		{
			sleep(0.2);
			close(pp.clientsocket);
			pp.clientsocket = -1;
			close(pp.serversocket);
			pp.serversocket = -1;
			return -1;
		}
		if(200 == hsc && tp)
			fwrite(buffer, recvsize, 1, store_fp); //add more packet to the file

		data_len += recvsize;
		ret = send(pp.clientsocket, buffer, recvsize, 0);
	}
	if(200 == hsc && tp)
		fclose(store_fp);

	sleep(0.2);
	close(pp.clientsocket);
	pp.clientsocket = -1;
	close(pp.serversocket);
	pp.clientsocket = -1;

	return 0;
}

int main(void)
{
	printf("proxy is starting... \n");
	printf("initing...\n");

	file_id = 0;
	fh_num = 0;
	read_init();
	pthread_mutex_init(&fid_mutex, NULL);
	
	int proxy_sfp;
	proxy_sfp = socket(AF_INET, SOCK_STREAM, 0);
	int on;
	setsockopt(proxy_sfp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(-1 == proxy_sfp)
	{
		printf("create proxy server socket failed!\n");
		return -1;
	}
	printf("create proxy server socket succeeded!\n");

	struct sockaddr_in ps_addr;
	memset(&ps_addr, 0, sizeof(struct sockaddr_in));
	ps_addr.sin_family = AF_INET;
	ps_addr.sin_addr.s_addr = INADDR_ANY;
	ps_addr.sin_port = htons(proxyport);

	if(-1 == bind(proxy_sfp, (struct sockaddr *)(&ps_addr), sizeof(struct sockaddr)))
	{
		printf("proxy server bind failed!\n");
		close(proxy_sfp);
		return -1;
	}
	printf("proxy server bind succeeded!\n");

	int somaxconn = 128; //the value in /proc/sys/net/core/somaxconn
	if(-1 == listen(proxy_sfp, somaxconn))
	{
		printf("proxy server listen failed!\n");
		close(proxy_sfp);
		return -1;
	}
	printf("proxy server is listening...\n");

	int sin_size = sizeof(struct sockaddr_in);
	int accept_fp;
	struct sockaddr_in accept_addr;
	while(1)
	{
		accept_fp = accept(proxy_sfp, (struct sockaddr *)(&accept_addr), &sin_size);
		setsockopt(accept_fp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if(-1 == accept_fp)
		{
			printf("proxy server accept failed!\n");
			continue;
		}
		printf("connect to the client: %s : %d\n", inet_ntoa(accept_addr.sin_addr), accept_addr.sin_port);
		pthread_t ntid;
		if(pthread_create(&ntid, NULL, (void *)proxythread, (void *)(&accept_fp)) != 0)
		{
			printf("create thread failed, connection is interrupted!\n");
			close(accept_fp);
			continue;
		}
	}
	
	pthread_mutex_destroy(&fid_mutex);
	close(proxy_sfp);
	return 0;
}
