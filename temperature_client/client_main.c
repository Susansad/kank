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

/*how to use this progject*/
static inline void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-h(--help):print this help information\n");
	printf("-p(--port):socket client port address\n");
	printf("-i(--ip):socket client ip address");
	printf("-t(--time):time intervals for data uploads in minutes\n");
	printf("example: %s -p 8900 -i 127.0.0.1 -t 30");
	return;
}

int main(int argc,char **argv)
{
	char			*progname=NULL;
	struct option	long_options[]=
	{
		{"port",required_argument,NULL,'p'},
		{"ip",required_argument,NULL,'i'},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	progname=basename=(argv[0]);
	/*parser the command line parameters*/
	while((opt=getopt_long(argv,argv,"p:i:t:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'p':
				client_port=atoi(optarg);
				break;
			case 'i':
				client_ip=optarg;
				break;
			case 't':
				time_interval=atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
		}
	}

}
