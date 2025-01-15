/********************************************************************************
 *      Copyright:  (C) 2024 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  temperature_client.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(11/12/24)
 *         Author:  LingYun <iot25@lingyun>
 *      ChangeLog:  1, Release initial version on "11/12/24 10:02:09"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <ctype.h>
#include <getopt.h>
#include <libgen.h>


int temperature_get(float *temp);
//int chipset_serial_number_get(char *sn);
int socket_client_init(char *ip,int port);
//int socket_client_write(char *ip,int port,char *msg_buf)
int init_db();
int insert_data(float temperature);
int	clear_db();
int send_data();


