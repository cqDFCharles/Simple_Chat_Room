#include "gdf.h"

int  sockserver(unsigned short port){
	struct sockaddr_in servaddr;
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1) ERR_EXIT("socket");
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int len = -1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,NULL,len);
	if(bind(listenfd,(SA *)&servaddr,sizeof(servaddr)) == -1) 
		ERR_EXIT("bind");
	listen(listenfd,10);
	return listenfd;
}
