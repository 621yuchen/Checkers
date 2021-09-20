/*client*/

#include"login.h"

#include"game.h"

#include<stdio.h>  
#include<stdlib.h>  
#include<netinet/in.h>  
#include<sys/socket.h>  
#include<arpa/inet.h>  
#include<string.h>  
#include<unistd.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <regex.h>
#define BUFFER_SIZE 1024  
#define LEN 20  

#define LOGIN 0
#define SIGNUP 1
#define ASK_CONNECT_TOSERVER 2
#define REPONSE_CONNECT_TOSERVER 3
#define DECIDE_TURN 4
#define REPONSE_LOGIN 5
#define REPONSE_SIGNUP 6
#define ASK_CONNECT_TOCLIENT 7
#define PROPOSE_CONNECT 9
#define AFFICHAGE 10
#define START 11
#define MOVE 12
#define PLAY 13
#define END 14


void confirmer_connexion(int server_sock_fd,char * recv){
	char recv_msg[BUFFER_SIZE];
	bzero(recv_msg,BUFFER_SIZE);
	strcpy(recv_msg,recv);
	char * split = ",";
	char * sender = strsep(&recv,split);
	char * reciever = recv;
	char choix[5];
	char input_msg[BUFFER_SIZE];
	printf("%s wanna play with you! Enter 'yes' to agree, enter 'no' to refuse!\n",sender);
	fgets(choix,5,stdin);
	if(strcmp(choix,"yes\n")==0){
		bzero(input_msg,BUFFER_SIZE);
		strcpy(input_msg,definir_msg(input_msg,"3:","true"));
		strcpy(input_msg,definir_msg(input_msg,",",sender));
		strcpy(input_msg,definir_msg(input_msg,",",reciever));
		if(send(server_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
			printf("client send error!\n");
		}
	}
	else if(strcmp(choix,"no\n")==0){
		bzero(input_msg,BUFFER_SIZE);
		strcpy(input_msg,definir_msg(input_msg,"3:","false"));
		strcpy(input_msg,definir_msg(input_msg,",",sender));
		strcpy(input_msg,definir_msg(input_msg,",",reciever));
		if(send(server_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
			printf("client send error!\n");
		}
	}
	else{
		printf("Illegal input!\n");
		confirmer_connexion(server_sock_fd,recv_msg);
	}

}

int main(int argc, const char * argv[]){  
	struct sockaddr_in server_addr;  
	server_addr.sin_family = AF_INET;  
	server_addr.sin_port = htons(22222);  
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  
	bzero(&(server_addr.sin_zero), 8);  

	int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);  
	if(server_sock_fd == -1){  
		perror("socket error!");  
		return 1;  
	}  
	char recv_msg[BUFFER_SIZE];  
	char input_msg[BUFFER_SIZE]; 

	//demander à connecter
	if(connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == 0){  

		/********************************* login ***************************************/

		login(server_sock_fd);
		enum cell_contents gameboard[BOARDWIDTH][BOARDHEIGHT];
		init_gameboard(gameboard);

		/**************	sent to serveur	************************************************/
		fd_set client_fd_set;  
		struct timeval tv;  
		while(1){  
			tv.tv_sec = 20;  
			tv.tv_usec = 0;  
			FD_ZERO(&client_fd_set);  
			FD_SET(STDIN_FILENO, &client_fd_set);  
			FD_SET(server_sock_fd, &client_fd_set);  

			select(server_sock_fd + 1, &client_fd_set, NULL, NULL, &tv);  
			if(FD_ISSET(STDIN_FILENO, &client_fd_set)){  
				bzero(input_msg, BUFFER_SIZE);  
				fgets(input_msg, BUFFER_SIZE, stdin);  
				if(strcmp(input_msg,"quit\n") == 0){
					close(server_sock_fd);
					exit(0);
				}
				if(send(server_sock_fd,input_msg,BUFFER_SIZE, 0) == -1){  
					perror("send error!\n");  
				}  
			} 


		/****************** recive from serveur ************************************************/
			if(FD_ISSET(server_sock_fd, &client_fd_set))  
			{  

				bzero(recv_msg, BUFFER_SIZE);
				long byte_num = recv(server_sock_fd, recv_msg, BUFFER_SIZE, 0);  
				if(byte_num > 0)  {  
					if(byte_num > BUFFER_SIZE)  {  
						byte_num = BUFFER_SIZE;  
					}  
					char * recv;
					char * split=":";
					recv = recv_msg;
					int type = atoi(strsep(&recv,split));

					switch(type){
						case REPONSE_LOGIN :
							if(strcmp(recv,"true")==0){
								printf("Wellcome!\n");
							}
							else{
								printf("Username or password is inccorect! Please try again!\n");
								login(server_sock_fd);
							}
							break;
						case REPONSE_SIGNUP :
							if(strcmp(recv,"true")==0){
								printf("Wellcome!\n");
							}
							else{
								printf("Username has been used!\n");
								login(server_sock_fd);
							}
							break;
						case PROPOSE_CONNECT :
							printf("%s\n",recv);
							char choix_adversaire[LEN];
							fgets(choix_adversaire,LEN,stdin);
							if(choix_adversaire[strlen(choix_adversaire)-1]=='\n'){
								choix_adversaire[strlen(choix_adversaire)-1] = '\0';
							}
							if(strcmp(choix_adversaire,"a")==0){
								printf("Please be patient... ...\n");
								break;
							}
							else{
								bzero(input_msg,BUFFER_SIZE);
								strcpy(input_msg,definir_msg(input_msg,"2:",choix_adversaire));
								if(send(server_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
									printf("client send error!\n");
								}
								else{printf("Your request was sent successfully! Please wait patiently... ...\n");
								printf("input_msg:%s\n",input_msg);
								}
								break;
							}

						case ASK_CONNECT_TOCLIENT :
							confirmer_connexion(server_sock_fd,recv);

							break;
						case DECIDE_TURN :
							split=",";
							char is_turn[10];
							bzero(is_turn,10);
							strcpy(is_turn,strsep(&recv,split));
							char col[10];
							bzero(col,10);
							strcpy(col,recv);
							enum colour is_col;
							if(strcmp(col,"white") == 0){
								is_col = P_WHITE;
							}
							else{
								is_col = P_RED;
							}
							/*printf("test for winner : %d\n",test_for_winner(gameboard,is_col));
							if(test_for_winner(gameboard,is_col)==1){*/
								if(strcmp(is_turn,"true")==0){
									printf("Your color is %s\n",col);
									printf("It's your turn!\n"); 
									int x1=-1;
									int x2=-1;
									int y1=-1;
									int y2=-1;
									char deplacement[50];
									int status = -1;
									do{  
										printf("Please enter your move:'x1,y1-x2,y2'\n");
										bzero(deplacement,50);
										if(scanf("%d,%d-%d,%d",&x1,&y1,&x2,&y2)==4){
											sprintf(deplacement,"%d,%d,%d,%d",x1,y1,x2,y2);
											status = send_move(deplacement,is_col,gameboard);
										}
										else{
											scanf("%s",deplacement);
											if(strcmp(deplacement,"quit")==0){
												close(server_sock_fd);
												exit(0);
											}
										}
										if(status==1){
											bzero(input_msg,BUFFER_SIZE);
											sprintf(deplacement,"%d,%d,%d,%d",x1,y1,x2,y2);
											strcpy(input_msg,definir_msg(input_msg,"12:",deplacement));
											if(send(server_sock_fd,input_msg,BUFFER_SIZE,0)==-1){
												printf("client send error!\n");
											}
											break;
										}
										else{

											printf("Illegal action! Please remove!\n");
										}
									}while(status==-1);
								}

								else{
									split = ",";
									char col[10];
									bzero(col,10);
									strcpy(col,recv);
									printf("Your color is %s\n",col);
									printf("Your adversary is moving! Please wait patiently... ...\n");
								}
							/*}
							else{
								bzero(input_msg,BUFFER_SIZE);
								strcpy(input_msg,definir_msg(input_msg,"14:","end"));
								if(send(server_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
									printf("client send error!\n");
								}
							}*/
							break;
						case AFFICHAGE :
							printf("show:%s\n",recv);
							break;
						case START :{
							display_gameboard(gameboard);
							break;
						}
						case PLAY :{
							if(strcmp("normal",strsep(&recv,split))==0){
								char deplacement[LEN];
								bzero(deplacement,LEN);
								strcpy(deplacement,strsep(&recv,split));
								if(strcmp(recv,"red") == 0){
									play_game(P_RED,gameboard,NORMAL,deplacement);
								}
								else{
									play_game(P_WHITE,gameboard,NORMAL,deplacement);
								}
							}
							else{    //attack
								char deplacement[LEN];
								bzero(deplacement,LEN);
								strcpy(deplacement,strsep(&recv,split));
								if(strcmp(recv,"red") == 0){
									play_game(P_RED,gameboard,ATTACK,deplacement);
								}
								else{
									play_game(P_WHITE,gameboard,ATTACK,deplacement);
								}
							}

							display_gameboard(gameboard);
							break;
						}
						default:
							printf("default:%s\n",recv);
					}


					//printf("server:%s\n", recv_msg);  
				}  
				else if(byte_num < 0)  {  
					printf("recive error!\n");  
				}  
				else  {  
					printf("server exit!\n");  
					exit(0);  
				}  
			} 
		}    
	}  

	return 0;  
} 
