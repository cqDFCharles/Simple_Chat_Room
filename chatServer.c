#include "gdf.h"

typedef struct socketnode{
				int sockfd;
				struct socketnode * next;
				struct socketnode * prev;
}socketnode, *socketlist;
typedef struct cliinfo{
	struct sockaddr_in cliaddr;
	int confd;
	socketlist socknode;
}cliinfo;
socketlist sockheadp,socktailp;
pthread_t tids[1024];
int cnt_threads = 0;
void echoSockfds();
void sendAllSockets(char *,cliinfo);
void * echocli(void * cli){
		char buf[1024];
		cliinfo info = *(cliinfo*)cli;
		socketlist releasep;
		printf("received connection from ip=%s\t port=%d\n",inet_ntoa(info.cliaddr.sin_addr),ntohs(info.cliaddr.sin_port));

		memset(buf,0,sizeof(buf));
		while(read(info.confd,buf,sizeof(buf)) > 0) {
			sendAllSockets(buf,info);
			memset(buf,0,sizeof(buf));
		}
		printf("ip=%s\t port=%d disconnected\n",inet_ntoa(info.cliaddr.sin_addr),ntohs(info.cliaddr.sin_port));
		//releasep->prev->next = releasep->next;
		//releasep->next->prev = releasep->prev;
		releasep = info.socknode;
		if(releasep == NULL){printf("releasep == NULL\n");return NULL;}
		socketlist node = releasep->prev;
		node->next = releasep->next;
		if(node->next != NULL){
			node->next->prev = node;
		}else{
						socktailp = node;
		}
						printf("socktailp->fd = %d\n",socktailp->sockfd);
						releasep->next = NULL;
		free(releasep);
		echoSockfds();

		return NULL;
}
int main(){
	int confd;
	struct sockaddr_in cliaddr;
	int i = 0;
	int listenfd = sockserver(8888);
	printf("listen in port 8888\n");
	socklen_t clilen = sizeof(cliaddr);
	sockheadp = (socketlist)malloc(sizeof(socketnode));
	socktailp = sockheadp;
	for(;;i++){
		char buf[1024];
		cliinfo cli;
		confd = accept(listenfd,(SA *)&cliaddr,&clilen);
		socketlist sockp = (socketlist)malloc(sizeof(socketnode));
		sockp->sockfd = confd;
		sockp->prev = socktailp;
		socktailp->next = sockp;
		socktailp = sockp;
		cli.socknode = sockp;
		cli.confd = confd;
		cli.cliaddr = cliaddr;
		cnt_threads++;
		echoSockfds();
		pthread_create(tids+i,NULL,echocli,&cli);
	}
	return 0;
}

void echoSockfds(){
		socketlist p = sockheadp->next;
		while(p != NULL){
						printf("%d->",p->sockfd);
						p = p->next;
		}
		printf("\n");

}
void sendAllSockets(char * buf,cliinfo info){
			socketlist sockp = sockheadp->next;
			while(sockp != NULL){
				if(sockp->sockfd != info.confd){
				 write(sockp->sockfd,buf,strlen(buf));
				 //memset(buf,0,sizeof(buf));
				}
				sockp = sockp->next;
			}

}
