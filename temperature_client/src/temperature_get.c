/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  temperature_get.c
 *    Description:  This file to get temperature
 *                 
 *        Version:  1.0.0(11/12/24)
 *         Author:  machenfeng
 *      ChangeLog:  1, Release initial version on "11/12/24 09:58:27"
 *                 
 ********************************************************************************/
#include "temperature_client.h"
#define BUFFER_SIZE 128

//char                path[50]="/sys/bus/w1/devices/";


 
int temperature_get(float *temp)
{
	char				path[50]="/sys/bus/w1/devices/";
	DIR                 *dirp;
	struct dirent       *direntp;
	int                 found=0;
	char				chip[20];
	char				buf[128];
	int					fd=-1;
	char				*ptr;
//	float				value;

	if(!temp)
	{
		return -1;
	}
	if((dirp=opendir(path))==NULL)
	{
		printf("open file error:%s\n",strerror(errno));
		return -1;
	}

	while((direntp=readdir(dirp))!=NULL)
	{
	//	printf("d_name: %s\n", direntp->d_name);
	//	printf("d_name: %lu\n", direntp->d_ino);
	//	printf("d_name: %ld\n", direntp->d_off);
	//	printf("d_name: %hu\n", direntp->d_reclen);
		//find chipset sn
		if(strstr(direntp->d_name, "28-"))
		{
			strcpy(chip,direntp->d_name);
			found=1;
			break;
		}
	}
	closedir(dirp);
	if(found<0)
	{
		printf("can not find ds18b20 in %s\n",path);
		return -2;
	}
	strncat(path,chip,sizeof(path)-strlen(path));
	strncat(path,"/w1_slave",sizeof(path)-strlen(path));

	if((fd=open(path,O_RDONLY))<0)
	{
		printf("open %s error: %s\n",path,strerror(errno));
		return -3;
	}
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("read %s error: %s\n",path,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("ERROR:Can not get temperature\n");
		return -5;
	}
	ptr+=2;
	*temp=atof(ptr)/1000.0;
	close(fd);
	return 0;



}


/*init socket*/
int	socket_client_init(char *ip,int port)
{
	int					connf_fd=-1;
	int					rv=0;
	struct	sockaddr_in	servaddr;
	connf_fd=socket(AF_INET,SOCK_STREAM,0);
	if(connf_fd<0)
	{
		printf("creat socket failure :%s\n",strerror(errno));
		return -1;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if(inet_aton(ip,&servaddr.sin_addr)<0)
	{
		printf("inet faild:%s",strerror(errno));
		rv=-2;
		return -3;
		goto Cleanup;
	}
	if((connect(connf_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)))<0)
	{
		printf("connet to server[%s:%d] failure:%s\n",ip,port,strerror(errno));
		rv=-1;
		goto Cleanup;
	}


Cleanup:
	if(rv<0)
		close(connf_fd);
	else rv=connf_fd;
	

	return rv;
}



//write message to ip port
int	socket_client_write(char *ip,int port,char *msg_buf,int conn_fd)
{
	if(write(conn_fd,msg_buf,strlen(msg_buf))<0)
	{
		printf("write data to server[%s:%d] failure:%s\n",ip,port,strerror(errno));
		close(conn_fd);
		return -2;
	}
	return 1;

}

