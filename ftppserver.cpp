#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <sstream>
#include <fstream>
 
#include <sys/sendfile.h>
#include <fcntl.h>
#include<iostream>
#include<string.h>
using namespace std;

int main(int argc, char *argv[])
{
    
    int listenfd = 0, connfd = 0,k;
    struct sockaddr_in serv_addr , client; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);         // create socket 
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);              // ip address
    serv_addr.sin_port = htons(atoi(argv[1]));                      // port which in input 

    k = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));        


    k = listen(listenfd, 10); 

    

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);        // accept the connection of client 
     printf("connected to the client\n");

	char buf[100];
	char command[100];
	char user[100];
	int ch=0;
	int passive = 0;
	int login = 0;
	while(!ch)
	{
		if(passive)
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		recv(connfd,buf,100,0);
		sscanf(buf,"%s",command);
		//printf("\nCommand : %s\n",command);

		if(strcmp("USER",command)==0)
		{
			recv(connfd,buf,100,0);
			sscanf(buf,"%s\n",user);
			if (!strcmp(user,"meghna")){
				login++;
                		printf("\n331 User name ok, need password.");
				printf("\nUSER : %s\n",user);
				
			}
            		else
 	               		printf("\n332 User name Invalid,Need account for login.\n");
			printf("\n\n---------------------------------------------------------------\n");
			
		}
		else if(strcmp("PASS",command)==0)
		{
			char pass[100];
			recv(connfd,buf,100,0);
			sscanf(buf,"%s\n",pass);
			if(strcmp(pass,"hello123"))
				printf("\nWrong password!\n");
			else{
				login++;
				printf("\nPassword ok;User logged in.");
				printf("\nPASSWORD : %s\n",pass);
				
				
				
			}

			printf("\n\n---------------------------------------------------------------\n");

		}
		else if(strcmp("QUIT",command)==0)
		{
			if(login == 2)
			{
			close(connfd);
			close(listenfd);
			ch=1;
			}
			else
				printf("\n530 Not logged in...(Send username and password to server)\n");

			

		}
		else if(strcmp("RETR",command)==0)
		{
			if(login == 2)
			{
			char filename[100];
			recv(connfd,buf,100,0);
			sscanf(buf,"%s\n",filename);
            		FILE* file = fopen(filename,"rb");
			int size;
            		if (!file){
                		size=0;
				send(connfd,&size,sizeof(int),0);
				printf("\nFile not available..\n");
            		}

            		else{

                		printf("150 File status okay\n");
				size = 1;
				send(connfd,&size,sizeof(int),0);
				char c;
				int words = 0;
				char buffer[100];
                		while((c=getc(file))!=EOF)
				{
					fscanf(file,"%s",buffer);
					if(isspace(c) || c == '\t')
						words++;
				}
				write(connfd,&words,sizeof(int));
				rewind(file);
				
				char ch;
				while(ch!=EOF)
				{
					fscanf(file," %s",buffer);
					write(connfd,buffer,512);
					ch = fgetc(file);
				}
				printf("\nThe file was sent successfully...\n");	
					
			}
			}
			else
				printf("\n530 Not logged in...(Send username and password to server)\n");

			printf("\n\n---------------------------------------------------------------\n");
			
		}
		else if(strcmp("STOR",command)==0)
		{
			if(login == 2)
			{
			char buffer[100];
			FILE *fp;
         		int ch = 0;
            		fp = fopen("glad_receive.txt","a");            
            		int words;
			read(connfd, &words, sizeof(int));
            		//printf("Passed integer is : %d\n" , words);      
          		while(ch != words)
       	   		{
        	 		read(connfd , buffer , 512); 
	   	 		fprintf(fp , " %s" , buffer);   
		 		//printf(" %s %d "  , buffer , ch); 
				ch++;
	   		}
			printf("\nThe new file created is glad_receive.txt");
     			printf("\nThe file was received successfully\n");
			}
			else
				printf("\n530 Not logged in...(Send username and password to server)\n");

			printf("\n\n---------------------------------------------------------------\n");
	   		
		}
		else if(strcmp("ABOR",command)==0)
		{
			passive = 1;
			printf("\nConnection Closed...");
			close(connfd);
			
			printf("\n\n---------------------------------------------------------------\n");

		}
		else if(strcmp("LIST",command)==0)
		{
			if(login == 2)
			{
			printf("(LIST):SHOW\n");
            		printf("125 transfer starting\n");

            		//struct sockaddr_in sa;
            		//socklen_t sa_len = sizeof(sa);
            		char buffer[1000];
            		std::string mostrar;
            		std::string ls = "ls";

            		ls.append(" 2>&1");

            		FILE* file = popen(ls.c_str(), "r");

            		if (!file){
                		printf("450 Requested file action not taken. File unavaible.\n");
                		
            		}

            		else{

                 		

                		while (!feof(file)){
                    			if (fgets(buffer,1000, file) != NULL) 
                        			mostrar.append(buffer);
				}

                		send(connfd, mostrar.c_str(), mostrar.size(), 0);

                		printf("250 Requested file action successful.\n");
                		pclose(file);
                		
			}
			}
			else
				printf("\n530 Not logged in...(Send username and password to server)\n");

			printf("\n\n---------------------------------------------------------------\n");

	
		}

	}
	
}

