/*2018.04.22
각 node들이 실행해야하는 코드 정리*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

typedef struct{
 	int dest;
    int link;
    int metric;
 }TABLE;

pthread_t tids[100];
int thds=0;
void * calculate(void*);
static void * handle(void *);
FILE* rfile;
TABLE* t;
//main
int main(int argc, char *argv[]){
	int srv_sock,cli_sock;
	int port_num, ret, node_num;
	struct sockaddr_in addr;
	int len,rc;
	int* p_num = &port_num;
	t->dest = 3333;
	t->link = 3333;
	t-> metric = 1;
//input값 잘못 들어온 경우
	if(argc != 4)	printf("excute form : ./com input.txt (port number) (node number)\n");
//input으로 들어온 대로 자신의 port번호 지정
	port_num = atoi(argv[2]);
//input으로 들어온 대로 노드 개수 저장
	node_num = atoi(argv[3]);
//input파일을 받아서 자신의 table정보 채우기
	//file 읽기
	rfile = fopen((char *) argv[1], "r");
	if(rfile == NULL){
		printf("there are no such file\n");
		exit(0);
	}
	else printf("read input file\n");

//다른 process열릴때까지 기다리기5초
	sleep(5);

	//file space대로 끈어서 각 table 요소에 저장

	//쓰레드로 calculate실행 -> 다이스트라 알고리즘
	//쓰레드 열기
	printf("do dijstra\n");
	pthread_create(&tids[thds], NULL, calculate,(void*) p_num);

//server로써의 역할
	//socket열기
	srv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_sock == -1) {
		perror("Server socket CREATE fail!!");
		return 0;
	}

//addr binding
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htons (INADDR_ANY); // 32bit IPV4 addr that not use static IP addr
	addr.sin_port = htons (port_num); // using port num

	ret = bind(srv_sock, (struct sockaddr *)&addr, sizeof(addr));	
	if (ret == -1) {
		perror("BIND error!!");
		close(srv_sock);
		return 0;
	}
	//listen
	for (;;){
	// Listen part
		ret = listen(srv_sock, 0);
		if (ret == -1) {
			perror("LISTEN stanby mode fail");
			close(srv_sock);
			return 0;	
		}
	
	//요청오면 accept -> cli저장
		cli_sock = accept(srv_sock, (struct sockaddr *)NULL, NULL); // client socket
		if (cli_sock == -1) {
			perror("cli_sock connect ACCEPT fail");
			close(srv_sock);
		}
		thds++;
	
	// cli handler
	//thread를 통해 cli와 통신 -> thread 실행 함수 -> handle
		pthread_create(&tids[thds], NULL, handle, &cli_sock);

	//만약모든 node에게 정보 주었다면 
		if(thds == node_num){ 
			close(srv_sock);
			break;
		}
	}// end for

//모든 thread가 끝날때까지 기다리기
	for(int i=0;i<=thds;i++){
		rc = pthread_join(tids[i], (void**)&ret);
		printf("join all thread");
	}
	return 0;
}

//handle
//the process send data with thread that execute handle
static void * handle(void * arg){
	int cli_sockfd = *(int *)arg;
	int len;
	int ret = -1;
	char *recv_buffer = (char *)malloc(1024);
	char *send_buffer = (char *)malloc(1024);
	
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int port_num =*((int*)arg);
	/* get peer addr */
	struct sockaddr peer_addr;
	socklen_t peer_addr_len;
	memset(&peer_addr, 0, sizeof(peer_addr));
	peer_addr_len = sizeof(peer_addr);
	ret = getpeername(cli_sockfd, &peer_addr, &peer_addr_len);
	ret = getnameinfo(&peer_addr, peer_addr_len, 
		hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 
		NI_NUMERICHOST | NI_NUMERICSERV); 

	if (ret != 0) {
		ret = -1;
		pthread_exit(&ret);
	}

	/* read from client host:port */
	len = recv(cli_sockfd, recv_buffer, sizeof(recv_buffer), 0);
	if (len == 0){
		ret = 0;
		pthread_exit(&ret);
	}
	
	/*send data to client*/
//	memset(send_buffer, 0, sizeof(send_buffer));
//	sprintf(send_buffer, "this is initial table of %d\n", port_num);
//	len = strlen(send_buffer);
	
	send(cli_sockfd, t, sizeof(t), 0);
//	send(cli_sockfd, send_buffer, len, 0);
	close(cli_sockfd);
	free(recv_buffer);
	free(send_buffer);
	
	ret = 0;
	pthread_exit(&ret);
}

//다이스트라 알고리즘
void* calculate(void* arg){
	int fd_sock, cli_sock;
	int port_num, ret;
	struct sockaddr_in addr;
	int len;
	size_t getline_len;
	char* cur_net;
	char *buffer;
//	char r_buffer[1024];
	TABLE* r_buffer;
	int flag = 0;
	int ser_port = 0;
sleep(5);	
	port_num = *((int*)arg);
	printf("port_num : %d", port_num);

	while(1){
		if(flag == 1){
			printf("client\n");	
			// socket creation
			fd_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (fd_sock == -1) {
				perror("socket");
				return 0;
			}
			cur_net = "127.000.000.001";
			ser_port = 3434;
			// addr binding, and connect
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons (ser_port);
			inet_pton(AF_INET, cur_net, &addr.sin_addr);
		
			ret = connect(fd_sock, (struct sockaddr *)&addr, sizeof(addr));
			if (ret == -1) {
				perror("connect");
				close(fd_sock);
				return 0;
			}
	
			buffer = "want connect";
			len = strlen(buffer);
			send(fd_sock, buffer, len, 0);

			memset(r_buffer, 0, sizeof(r_buffer));
			len = recv(fd_sock, r_buffer, sizeof(r_buffer), 0);
	
//			printf("server says $ %s\n", r_buffer);
			printf("server says $ %d %d %d\n", r_buffer -> dest, r_buffer -> link, r_buffer -> metric);
			fflush(NULL);
	
			close(fd_sock);
			return 0;
		}//if end
		if(port_num == 3333)flag=1;
	}//while end
}