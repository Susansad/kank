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


char                path[50]="/sys/bus/w1/devices";



/*get chipset serial number*/
int chipset_serial_number_get(char *sn)
{
	DIR					*dirp;
	struct dirent		*direntp;
	int					found=-1;
//open dierectory to get chipset serial number
	if((dirp=opendir(path))==NULL)
	{
		printf("open file error:%s\n",strerror(errno));
		return -1;
	}

	while((direntp=readdir(dirp))!=NULL)
	{
		//find chipset sn
		if(strstr(direntp->d_name,"28-"))
		{
			strcpy(sn,direntp->d_name);
			found=-1;
			break;
		}
	}
	closedir(dirp);

	if(found<0)
	{
		printf("can not find ds18b20 in %s\n",path);
		return -2;
	}

}

int temperature_get(float *temp)
{
	char				sn[20];
	char				w1_path[50];
	char				buf[128];
	int					fd=-1;
	int					found=-1;
	char				*ptr;
	float				value;
	//Determine if the temperature is correctly transmitted
	if(!temp)
	{
		return -1;
	}
	if(chipset_serial_number_get(sn)<0)
	{
		printf("get chipset serial number faliure\n");
		return -2;
	}
	//get ds18b20 full path
	strcpy(w1_path,path);
	strncat(w1_path,sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"w1_slave",sizeof(w1_path)-strlen(w1_path));

	//open file to get temperature
	if((fd=open(w1_path,O_RDONLY))<0)
	{
		printf("open %s error: %s\n",w1_path,strerror(errno));
		return -3;
	}
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("read %s error: %s\n",w1_path,strerror(errno));
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
	
