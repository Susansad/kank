/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun<iot25@lingyun>
 *                  All rights reserved.
 *
 *       Filename:  sqlite.c
 *    Description:  This file to init sqlite 
 *                 
 *        Version:  1.0.0(17/12/24)
 *         Author:  machenfeng <iot25@lingyun>
 *      ChangeLog:  1, Release initial version on "17/12/24 10:19:22"
 *                 
 ********************************************************************************/

#include "temperature_client.h"

#define DB_FILE "data.db"
sqlite3 	*db;
extern int	socketfd;
pthread_mutex_t	db_mutex=PTHREAD_MUTEX_INITIALIZER;
extern int 	server_socket;
int init_db()
{
	char 	*err_msg=0;
	const char 	*sql="CREATE TABLE IF NOT EXISTS DATA (id INTEGER PRIMARY KEY,value REAL,timestamp TEXT DATETIME DEFAULT CURRENT_TIMESTAMP);";
	if(sqlite3_open(DB_FILE,&db))
	{
		fprintf(stderr,"can not open database:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(sqlite3_exec(db,sql,0,0,&err_msg)!=SQLITE_OK)
	{
		fprintf(stderr,"SQL init ERROR :%s\n",err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -2;
	}
	return 1;
}

//insert data from database
int insert_data(float temperature)
{
	char 		*err_msg=NULL;
	char		*sql=NULL;
	
	pthread_mutex_lock(&db_mutex);
	sql=sqlite3_mprintf("INSERT INTO data (value) VALUES (%f);",temperature);
	if((sqlite3_exec(db,sql,0,0,&err_msg))!=SQLITE_OK)
	{
		fprintf(stderr,"SQL exec ERROR :%s\n",err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(sql);
		pthread_mutex_unlock(&db_mutex);
		return -1;
	}
	sqlite3_free(sql);
	pthread_mutex_unlock(&db_mutex);
	return 1;
}

int send_data()
{
	pthread_mutex_lock(&db_mutex);
	sqlite3_stmt			*res;
	int					id;
	double				value;
	const unsigned char* timestamp;
	char				buffer[128];
	const char *sql="SELECT id,value,timestamp FROM data;";
	if(sqlite3_prepare_v2(db,sql,-1,&res,0)==SQLITE_OK)
	{
		while(sqlite3_step(res)==SQLITE_ROW)
		{
			id=sqlite3_column_int(res,0);
			value=sqlite3_column_double(res,1);
			timestamp=sqlite3_column_text(res,2);
			snprintf(buffer,sizeof(buffer),"ID: %d,Value: %f,Timestamp: %s\n",id,value,timestamp);
			send(socketfd,buffer,strlen(buffer),0);
				
		}
		sqlite3_finalize(res);
	}
	else
	{
		fprintf(stderr,"failed to ececute statement:%s\n",sqlite3_errmsg(db));
	}
	pthread_mutex_unlock(&db_mutex);
	return 1;
}

int clear_db()
{
	pthread_mutex_lock(&db_mutex);
	char	*sql="DELETE FROM data";
	char	*err_msg=NULL;
	if(sqlite3_exec(db,sql,0,0,&err_msg)!=SQLITE_OK)
	{
		fprintf(stderr,"SQL error:%s\n",err_msg);
		sqlite3_free(err_msg);
		pthread_mutex_unlock(&db_mutex);
		return -1;
	}
	pthread_mutex_unlock(&db_mutex);
	return 1;

}
