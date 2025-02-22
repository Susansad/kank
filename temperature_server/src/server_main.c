/*********************************************************************************
 *      Copyright:  (C) 2025 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  server_main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(15/01/25)
 *         Author:  machenfeng <iot25@lingyun>
 *      ChangeLog:  1, Release initial version on "15/01/25 10:43:27"
 *                 
 ********************************************************************************/

#include "server_socket.h"

#define MAX_EVENTS 512
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

int socketfd;

static inline void print_usage(char * progname);

int main(int argc,char **argv)
{
	int					listenfd;
	int					serv_port=0;
	int					daemon_run=0;
	char				*progname=NULL;
	int					opt;
	int					rv;
	int					i;
//	int					found;
	char				buf[1024];
	float				temperature;
	int					epollfd;
	struct epoll_event	event;
	struct epoll_event	event_array[MAX_EVENTS];
	int					events;

	struct option		long_options[]=
	{
		{"daemon",no_argument,NULL,'b'},
		{"port",required_argument,NULL,'P'},
		{"help",no_argument,NULL,'H'},
		{NULL,0,NULL,0}
	};

	progname=basename(argv[0]);

	while((opt=getopt_long(argc,argv,"bp:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'b':
				daemon_run=1;
				break;
			case 'p':
				serv_port=atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return	EXIT_SUCCESS;
			default:
				break;
		}
	}
	if(!serv_port)
	{
		print_usage(progname);
		return -1;
	}

	set_socket_rlimit();

	if((listenfd=socket_server_init(NULL,serv_port))<0)
	{
		printf("error:%s server listen on port %d failure\n",argv[0],serv_port);
		return -2;
	}

	printf("%s server start to listen on port %d\n",argv[0],serv_port);

	if(daemon_run)
	{
		daemon(0,0);
	}
	if((epollfd=epoll_create(MAX_EVENTS))<0)
	{
		printf("epoll creat failure:%s\n",strerror(errno));
		goto CleanUp;
		return -3;
	}

	event.events=EPOLLIN;
	event.data.fd=listenfd;

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event)<0)
	{
		printf("epoll add listen socket failure:%s\n",strerror(errno));
		goto CleanUp;
		return -4;
	}

	init_db();


	for(;;)
	{
		/* program will blocked here */
		events=epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if(events<0)
		{
			printf("epoll failure1:%s\n",strerror(errno));
			break;
		}
		else if(events==0)
		{
			printf("epoll failure2:%s\n",strerror(errno));
			break;
		}


		for(i=0;i<events;i++)
		{
			if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}

			/* listen socket get event means new client start */
			if(event_array[i].data.fd==listenfd)
			{
				if((socketfd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
				{
					printf("accept new client failure:%s\n",strerror(errno));
					continue;
				}

				event.data.fd=socketfd;
				event.events=EPOLLIN;
				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,socketfd,&event)<0)
				{
					printf("epoll add client socket failure:%s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}

				printf("epoll add new client socket[%d] ok.\n",socketfd);

			}
			else/* already conneted client socket get data incoming */
			{
				memset(buf,0,sizeof(buf));
				if((rv=read(event_array[i].data.fd,buf,sizeof(buf)))<=0)
				{
					printf("socket[%d] read failure or get disconnct and will be removed.\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{

					printf("socket[%d] read get %d bytes data:%s\n",event_array[i].data.fd,rv,buf);
					temperature=extract_value(buf,"Temperature");
					insert_data(temperature);
				}
			}
		}
	}
CleanUp:
	close(listenfd);
	return 0;
}

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n", progname); 
	printf(" %s is a socket server program, which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");   
	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port ] Socket server port address\n");
	printf(" -h[help ] Display this help information\n");	  
	printf("\nExample: %s -b -p 8900\n", progname);
	return ;
}

float extract_value(const char *data, const char *key)
{
	char *pos=strstr(data,key);
	if(pos!=NULL)
	{
		pos += strlen(key);
		while(*pos && (*pos == ':' || *pos == ' ' || *pos == '\t'))
		{
			pos++;
		}
		if(*pos)
		{
			return atof(pos);
		}
	}
	return 0.0;
}
