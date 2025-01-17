/*********************************************************************************
 *      Copyright:  (C) 2025 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  server_socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(15/01/25)
 *         Author:  LingYun <iot25@lingyun>
 *      ChangeLog:  1, Release initial version on "15/01/25 10:45:36"
 *                 
 ********************************************************************************/
#include "server_socket.h"

int socket_server_init(char *listen_ip,int listen_port)
{
	struct sockaddr_in		servaddr;
	int						rv=0;
	int						on=1;
	int						listenfd;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("use socket to creat a TCP socket failure:%s\n",strerror(errno));
		return -1;
	}

	/* set socket port reuseable,fix 'address already in use' */
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(listen_port);

	if(!listen_ip) /* listen all local ip */
	{
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else/* listen the specified ip */
	{
		if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr)<=0)
		{
			printf("inet_pton set liten ip failure\n");
			rv=-2;
			goto CleanUp;
		}
	}
	if(bind(listenfd,(struct sockaddr *) &servaddr,sizeof(servaddr))<0)
	{
		printf("use bind() to bind the TCP socket failure:%s\n",strerror(errno));
		rv=-3;
		goto CleanUp;
	}
	if(listen(listenfd,64)<0)
	{
		printf("use bind to TCP socket failure:%s\n",strerror(errno));
		rv=-4;
		goto CleanUp;
	}
CleanUp:
	if(rv<0)
		close(listenfd);
	else
		rv=listenfd;
	return rv;
}

/* set open file description count to max */
void set_socket_rlimit(void)
{
	struct	rlimit limit={0};
	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur	=	limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);
	printf("set socket open fd max count to %ld\n",limit.rlim_max);
}
