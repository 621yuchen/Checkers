#include "gameboard.h"



enum colour
{
    P_RED=0, P_WHITE
};

struct client {
	int client_sock_fd;
	char nom_utilisateur[20];
	enum colour col;
	int adversaire_sock_fd;
	char * turn;
};

enum move_type
{
    ATTACK, NORMAL, INVALID
};

int send_move(char * deplacement, enum colour col_client, enum cell_contents board[][BOARDWIDTH])
{
  char * split = ",";
  int ystart = atoi(strsep(&deplacement,split));
  int xstart = atoi(strsep(&deplacement,split));
  int yend = atoi(strsep(&deplacement,split));
  int xend = atoi(deplacement);

  if( xstart>=0 && ystart>=0 && xend>=0 && yend>=0 && xstart<=BOARDWIDTH-1 && ystart<=BOARDWIDTH-1 && xend<=BOARDWIDTH-1 && yend<=BOARDWIDTH-1){
    if( (board[ystart][xstart]==RED && col_client == P_RED && board[yend][xend]==EMPTY)
        ||  (board[ystart][xstart]==WHITE && col_client == P_WHITE && board[yend][xend]==EMPTY)
        ||  (board[ystart][xstart]==K_WHITE && col_client == P_WHITE && board[yend][xend]==EMPTY)
        ||  (board[ystart][xstart]==K_RED && col_client == P_RED && board[yend][xend]==EMPTY) ){
      return 1;
    }
    else if( (board[ystart][xstart]==RED && col_client == P_RED && board[yend][xend]==EMPTY && ( board[(ystart)-1][(xstart)+1]!=RED || board[(ystart)-1][(xstart)-1]!=RED ))
               ||  (board[ystart][xstart]==WHITE && col_client == P_WHITE && board[yend][xend]==EMPTY && ( board[(ystart)+1][(xstart)+1]!=WHITE || board[(ystart)+1][(xstart)-1]!=WHITE))
               ||  (board[ystart][xstart]==K_WHITE && col_client == P_WHITE && board[yend][xend]==EMPTY && ( board[(ystart)+1][(xstart)+1]!=RED || board[(ystart)+1][(xstart)-1]!=RED ))
               ||  (board[ystart][xstart]==K_RED && col_client == P_RED && board[yend][xend]==EMPTY && ( board[(ystart)-1][(xstart)+1]!=RED || board[(ystart)-1][(xstart)-1]!=RED ))){
      return 1;
    }
  }
  printf("Not allowed! \n");
  return -1;
}



enum move_type is_valid_move(char * deplacement)
{
    char * split = ",";
    int ystart = atoi(strsep(&deplacement,split));
    int xstart = atoi(strsep(&deplacement,split));
    int yend = atoi(strsep(&deplacement,split));
    int xend = atoi(deplacement);

    if( (abs(xend-xstart)==1 && abs(yend-ystart)==1) ){
      return NORMAL;
    }
    else if( (abs(xend-xstart)==2 && abs(yend-ystart)==2) ){
      return ATTACK;
    }
    else{
  	return INVALID;
    }
}




void player_turn(struct client * client_fds,struct client current, char * deplacement)
{

	char input_msg[BUFFER_SIZE];
	char dep_initial[50];
	bzero(dep_initial,50);
	strcpy(dep_initial,deplacement);
	enum move_type current_move_type;

	do {
		current_move_type = is_valid_move(deplacement);
		if(current_move_type == INVALID){
        		bzero(input_msg,BUFFER_SIZE);
			strcpy(input_msg,definir_msg(input_msg,"10:","This movement is not allowed! Please try again!"));
			if(send(current.client_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
				printf("client send error!\n");
			}
			bzero(input_msg,BUFFER_SIZE);
			strcpy(input_msg,definir_msg(input_msg,"4:","true")); // invalide
			if(current.col == P_RED){
				strcpy(input_msg,definir_msg(input_msg,",","red")); // invalide
			}
			else{
				strcpy(input_msg,definir_msg(input_msg,",","white")); // invalide
			}
			if(send(current.client_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
				printf("client send error!\n");
			}
		}	
		else{

			if(current_move_type == NORMAL){
				bzero(input_msg,BUFFER_SIZE);
				strcpy(input_msg,definir_msg(input_msg,"13:","normal"));//normal
				strcpy(input_msg,definir_msg(input_msg,":",dep_initial));
				if(current.col == P_RED){
					strcpy(input_msg,definir_msg(input_msg,":","red"));
				}
				else{
					strcpy(input_msg,definir_msg(input_msg,":","white"));
				}
				printf("normal movement : %s\n",input_msg);
				if(send(current.client_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
					printf("client send error!\n");
				}	
				if(send(current.adversaire_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
					printf("client send error!\n");
				}
			}
			else{ // ATTACK
				bzero(input_msg,BUFFER_SIZE);
				strcpy(input_msg,definir_msg(input_msg,"13:","attack")); //attack
				strcpy(input_msg,definir_msg(input_msg,":",dep_initial));
				if(current.col == P_RED){
					strcpy(input_msg,definir_msg(input_msg,":","red"));
				}
				else{
					strcpy(input_msg,definir_msg(input_msg,":","white"));
				}
				printf("attack deplacement : %s\n",input_msg);
				if(send(current.client_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
					printf("client send error!\n");
				}	
				if(send(current.adversaire_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
					printf("client send error!\n");
				}
			}
	
			bzero(input_msg,BUFFER_SIZE);
			strcpy(input_msg,definir_msg(input_msg,"10:","Your movement is valid!"));
			if(send(current.client_sock_fd,input_msg,BUFFER_SIZE, 0)==-1){
				printf("client send error!\n");
			}


			break;
		}
	} while ( 1 );

}

void play_game(enum colour col_client,enum cell_contents board[][BOARDWIDTH],enum move_type movetype,char * deplacement){
	enum cell_contents temp_cell;
	char * split = ",";
	int ystart = atoi(strsep(&deplacement,split));
	int xstart = atoi(strsep(&deplacement,split));
	int yend = atoi(strsep(&deplacement,split));
	int xend = atoi(deplacement);
	switch ( movetype ) {
	case NORMAL:
		temp_cell = board[ystart][xstart];
		board[ystart][xstart] = EMPTY;

		if(col_client == P_RED && yend == 7){
			board[yend][xend] = K_RED;
		}
		else if(col_client == P_WHITE && yend == 0){
			board[yend][xend] = K_WHITE;
		}
		else {
			board[yend][xend] = temp_cell;
		}
		break;

	case ATTACK:
		temp_cell = board[ystart][xstart];
		board[ystart][xstart] = EMPTY;
		board[(ystart + yend) / 2][(xstart + xend) / 2] = EMPTY;

		if(col_client == P_RED && yend == 7){
			board[yend][xend] = K_RED;
		}
		else if(col_client == P_WHITE && yend == 0){
			board[yend][xend] = K_WHITE;
		}
		else{
			board[yend][xend] = temp_cell;
		}

		break;
	default:
	;
	}
}

/* pas correcte */
int test_for_winner(enum cell_contents board[][BOARDWIDTH],enum colour col){
	int i,j,resultat;
	resultat = 1;
	int xstart,ystart,xend,yend;
	char possible_move[50];

	for( i = 0; i <  BOARDHEIGHT; i++ ){
		for (j = 0; j < BOARDWIDTH; j++) {
			if(board[i][j] == RED && col == P_RED){
				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}


				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

			}else if(board[i][j] == K_RED && col == P_RED){
				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

			}else if(board[i][j] == WHITE && col == P_WHITE){
				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

			}else if(board[i][j] == K_WHITE && col == P_WHITE){
				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i - 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j - 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}

				xstart = i;
				ystart = j;
				xend = i + 1;
				yend = j + 1;
				bzero(possible_move,50);
				sprintf(possible_move,"%d,%d,%d,%d",xstart,ystart,xend,yend);
				if (send_move(possible_move,col,board)==1){
					if(is_valid_move(possible_move)==INVALID){
						resultat = 0;
					}
					return resultat;
				}
				else{
					resultat = 0;
					return resultat;
				}
			}
		}
	}
	return resultat;
}
