#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

#define BUFFER_SIZE 1024

void bzero();
char * strsep();

char * definir_msg(char input_msg[],char protocole[],char data[]){
	strcat(input_msg,protocole);
	strcat(input_msg,data);
	return input_msg;
}


char * definir_compte(char input_msg[],char protocole[],char nom_utilisateur[],char password[]){
	bzero(input_msg,BUFFER_SIZE);
	strcpy(input_msg,protocole);
	strcat(input_msg,nom_utilisateur);
	strcat(input_msg,",");
	strcat(input_msg,password);
	return input_msg;
}

char * verifier_compte(FILE*fd,char * compte){
	static char nom_utilisateur[LEN];
	bzero(nom_utilisateur,LEN);
	if(fd==NULL){
		perror("Open fail");
		exit(1);
	}

	char string[50];

	fseek(fd,0,SEEK_SET);
	while(fgets(string,sizeof(string),fd)){
		if(string[strlen(string)-1]=='\n'){
			string[strlen(string)-1]='\0';
		}
		char * split=",";		
		if(strcmp(string,compte)==0){
			strcpy(nom_utilisateur,strsep(&compte,split));
			break;
		}

	}
	fclose(fd);
	return nom_utilisateur;
}


char * enregistrer_compte(FILE*fd,char * compte){
	static char nom_utilisateur[LEN];
	bzero(nom_utilisateur,LEN);
	char string[50];
	char str_info[50];
	char str_nom[LEN];

	strcpy(str_info,"\n");
	strcat(str_info,compte);
	char * split=",";		
	strcpy(nom_utilisateur,strsep(&compte,split));

	while(fgets(string,sizeof(string),fd)){
		if(string[strlen(string)-1]=='\n'){
			string[strlen(string)-1]='\0';
		}

		sscanf(string,"%[^,]",str_nom);
	
		if(strcmp(str_nom,nom_utilisateur)==0){
			bzero(nom_utilisateur,LEN);
			break;
		}
	}
	if(strlen(nom_utilisateur)!=0){
		fputs(str_info,fd);
	}
	fclose(fd);
	return nom_utilisateur;
}


void login(int server_sock_fd){
	char data[100];
	bzero(data, 100);
	char input_msg[BUFFER_SIZE];
	bzero(input_msg,BUFFER_SIZE);
	//Connexion
	printf("a.Log in\n");
	printf("b.Sign up\n");
	printf("c.Quit\n");

	char nom_utilisateur[LEN];
	char password[LEN];
	char choix[5];
	int statut = -1;

	do{
		printf("Please enter your choice:");
		fgets(choix,5,stdin);
		if(strcmp(choix,"a\n")==0){
			printf("username:");
			fgets(nom_utilisateur,LEN,stdin);
			if(nom_utilisateur[strlen(nom_utilisateur)-1]=='\n'){
				nom_utilisateur[strlen(nom_utilisateur)-1]='\0';
			}
			printf("password:");
			fgets(password,LEN,stdin);
			if(password[strlen(password)-1]=='\n'){
				password[strlen(password)-1]='\0';
			}
			statut = 0;
			strcpy(input_msg,definir_compte(input_msg,"0:",nom_utilisateur,password));
			if(send(server_sock_fd, input_msg, BUFFER_SIZE, 0) == -1){  
				perror("message send error!\n");  
			}
		}
		else if(strcmp(choix,"b\n")==0){
			printf("username:");
			fgets(nom_utilisateur,LEN,stdin);
			if(nom_utilisateur[strlen(nom_utilisateur)-1]=='\n'){
				nom_utilisateur[strlen(nom_utilisateur)-1]='\0';
			}
			while(strlen(nom_utilisateur)>15){
				printf("15 lettres maximum!");
				printf("username:");
				fgets(nom_utilisateur,LEN,stdin);
				if(nom_utilisateur[strlen(nom_utilisateur)-1]=='\n'){
					nom_utilisateur[strlen(nom_utilisateur)-1]='\0';
				}
			}

			printf("password:");
			fgets(password,LEN,stdin);
			if(password[strlen(password)-1]=='\n'){
				password[strlen(password)-1]='\0';
			}
			while(strlen(password)>15){
				printf("15 lettres maximum!");
				printf("password:");
				fgets(password,LEN,stdin);
				if(password[strlen(password)-1]=='\n'){
					password[strlen(password)-1]='\0';
				}
			}
			statut = 0;
			strcpy(input_msg,definir_compte(input_msg,"1:",nom_utilisateur,password));
			if(send(server_sock_fd, input_msg, BUFFER_SIZE, 0) == -1){  
				perror("message send error!\n");  
			}
		}
		else if(strcmp(choix,"c\n")==0){
			statut = 0;
			exit(0);
		}
		else{
			printf("Illegal input!\n");
		}
	}
	while(statut == -1);


}


