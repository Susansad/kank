/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  client_main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/12/24)
 *         Author:  LingYun <iot25@lingyun>
 *      ChangeLog:  1, Release initial version on "11/12/24 16:11:38"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include "temperature_client.h"

#define MAX_EVENTS 512
#define ARRAY_SIZE(X) (sizeof(x)/sizeof(x[0]))

int socketfd;
#define msg_buf	"DS18B20 get temperature:"
void *temperature_thread(void *arg);
void *report_thread(void *arg);


/*how to use this progject*/
static inline void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-h(--help):print this help information\n");
	printf("-p(--port):socket client port address\n");
	printf("-i(--ip):socket client ip address\n");
	printf("-t(--time):time intervals for data uploads in minutes\n");
	printf("example: %s -p 8900 -i 127.0.0.1 -t 30\n",progname);
	return;
}

int main(int argc,char **argv)
{
	char				*progname=NULL;
	char				*client_ip=NULL;
	int					client_port=0;
	int					time_interval=0;
//	int                 time_interval2=0;
	int					opt;
	float				temp;
	int					flag=0;
//	char				msg_buf[128]="DS18B20 get temperature:";
	time_t				rawtime;
	struct	tm			*timeinfo;
	char				buf[80];
	char				final_msg[80];
//	pthread_t			temp_thread;
//	pthread_t			rep_thread;

	struct option	long_options[]=
	{
		{"port",required_argument,NULL,'p'},
		{"ip",required_argument,NULL,'i'},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	progname=basename(argv[0]);
	/*parser the command line parameters*/
	while((opt=getopt_long(argc,argv,"p:i:t:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'p':
				client_port=atoi(optarg);
				break;
			case 'i':
				client_ip=optarg;
			//	printf("ip add:%s\n",client_ip);
				break;
			case 't':
				time_interval=atoi(optarg);
		//		time_interval2=atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
		}
	}

	if((!client_ip) || (!client_port) || (!time_interval))
	{
		print_usage(progname);
		return -1;
	}
	init_db();

	while(1)
	{
		if((temperature_get(&temp))<0)
		{
			printf("device get temperature failure\n");
			continue;
		}

//check socked connect
		socketfd=socket_client_init(client_ip,client_port);
		if(socketfd<0)
		{
			flag=1;
			if((insert_data(temp))<0)
			{
				printf("[%f] insert  to sqlite error\n",temp);
				continue;
			}
	
			printf("socket disconnet,data sorted into db\n");
			continue;
		}
		else
		{
			if(flag)
			{
				send_data();
				flag=0;
				clear_db();
			}

			//check msg
			memset(final_msg, 0, sizeof(final_msg));
			strncpy(final_msg,msg_buf, sizeof(final_msg) - 1);
			memset(&buf,0,sizeof(buf));
			sprintf(buf,"%f",temp);
//			printf("buf :%s\n",buf);
			strncat(final_msg,buf,sizeof(final_msg)-strlen(final_msg));
//			printf("msg:%s\n",final_msg);
			time(&rawtime);
			timeinfo=localtime(&rawtime);
			memset(&buf,0,sizeof(buf));
			strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",timeinfo);
//			printf("buf:%s\n",buf);
			strncat(final_msg," ",sizeof(final_msg)-strlen(final_msg));
			strncat(final_msg,buf,sizeof(final_msg)-strlen(final_msg));
			strncat(final_msg,"\n",sizeof(final_msg)-strlen(final_msg));
//			printf("msg:%s\n",final_msg);
			if(write(socketfd,final_msg,strlen(final_msg))<0)           
			{                                                      
				printf("write data to server[%s:%d] failure:%s\n",client_ip,client_port,strerror(errno));
				close(socketfd);                                    
				return -2;                                         
			}

		}
		sleep(time_interval);
	}
}
//	init_db();

	/*creat thread work
	if(pthread_create(&temp_thread,NULL,temperature_thread,(void *)(intptr_t)time_interval1)!=0)
	{
		printf("fail to creat temperature thread\n");
		close(socketfd);
		exit(EXIT_FAILURE);
	}
 
	if(pthread_create(&rep_thread,NULL,report_thread,(void *)(intptr_t)time_interval2)!=0)
	{
		printf("fail to creat report thread\n");
		close(socketfd);
		exit(EXIT_FAILURE);
	}


	pthread_join(temp_thread,NULL);
	pthread_join(rep_thread,NULL);
	if(clear_db()<0)
	{
		printf("close error\n");
		return -3;
	}
	close(socketfd);


}

void* temperature_thread(void* arg)
{
	int 	interval1=(int)arg;
	//free(arg);
	char	sn[16];
	float	temp=1;
	while(1)
	{
		
		if((temperature_get(&temp))<0)
		{
			printf("device [%s] get temperature failure\n",sn);
			continue;
		}

		if((insert_data(temp))<0)
		{
			printf("[%s:%f] insert  to sqlite error\n",sn,temp);
			continue;
		}
		sleep(interval1);
	}
	return NULL;
}

void *report_thread(void *arg)
{
	int		interval2=(int)arg;
//	printf("interval is %d",interval2);
	//free(arg);
	sleep(5);
	while(1)
	{
		if(send_data()<0)
		{
			printf("send data error\n");
			continue;
		}	
		sleep(interval2);
	  	if(clear_db()<0)
		{
			printf("clear db error\n");
			continue;
		}
	
	}


	return NULL;
}
*/
