#ifndef GDF_H
#define GDF_H

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <resolv.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <fcntl.h>
//system V 
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
//posix
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>

#define ERR_EXIT(m)	\
	do{								\
		perror(m);\
		exit(0);\
	}while(0)

#define MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define min(m,n) ( m > n ? n : m)
#define max(m,n) (m > n ? m : n)
#define MAXLINE 100
#define SA struct sockaddr
#endif

int sockserver(unsigned short);
int shm_readint(char * name,int *val);
int shm_writeint(char * name,int val);
