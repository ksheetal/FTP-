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
#include<cstring>
using namespace std;


/*
memset -  Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an unsigned char).
strcat() -  function in C++ appends a copy of a string to the end of another string.
*/


int main(int argc,char *argv[])
{
	int sockfd = 0,n = 0,connfd = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;
	
	if(argc != 3)
	{
		printf("\n Usage : %s <ip of server> \n",argv[0]);    //checking argument
		return 1;
	}
	
	memset(recvBuff,'0',sizeof(recvBuff));
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("\nError : Could not create socket \n");
		return 1;
	}
	memset(&serv_addr,'0',sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\ninet_pton error ocurred\n");
		return 1;
	}
	if(connfd=connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		printf("\nError : Connect failed\n");
		return 1;
	}

	char command[100];
	int choice;
	char arg[100];
	char ch = false;
	int passive = 0;
	int login = 0;
	while(!ch)
	{
		if(passive)
			connfd=connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		printf("\nEnter your command : ");
		scanf("%s",command);
		
		if(strcmp("USER",command)==0)
		{
			send(sockfd,command,100,0);
			printf("\nEnter user name : ");
			scanf("%s",arg);
			//printf("\nUSER: %s",arg);
			
			login++;
			send(sockfd,arg,100,0);
    			printf("\n\n---------------------------------------------------------------\n");
    		
		}
		else if(strcmp("PASS",command)==0)
		{
			send(sockfd,command,100,0);
			char pass[100];
			printf("\nEnter password : ");
			scanf("%s",pass);
			login++;
			
			
			send(sockfd,pass,100,0);
			printf("\n\n---------------------------------------------------------------\n");
		}
		else if(strcmp("PWD",command)==0)
		{
			send(sockfd,command,100,0);
			 printf("(PWD):SHOW\n");

            		char path[1000]; 

            		if (getcwd(path, sizeof(path)) != NULL)
                		printf("\"%s\" \n", path);
			printf("\n\n---------------------------------------------------------------\n");

		}

		else if(strcmp("MKD",command)==0)
		{
			send(sockfd,command,100,0);
			printf("\n\nEnter directory name: ");
            		scanf("%s", arg);
            		printf("(MKD):%s\n", arg);

            		if (mkdir(arg, 0755) < 0)
                		printf("\n550 Failed to create directory.\n");
            		else
                		printf("\n257 Directory successfully created.\n"); 
			printf("\n\n---------------------------------------------------------------\n");
        	}

		else if (strcmp("CWD",command)==0)
		{
			send(sockfd,command,100,0);
			printf("\n\nEnter path name : ");
            		scanf("%s", arg);
            		printf("(CWD):%s\n", arg);

            		char path[100];

            		if (getcwd(path, sizeof(path)) != NULL)
			{

                		strcat(path,"/");
                		strcat(path,arg);

                		if (chdir(path) < 0)
                    			printf("\n550 Failed to change directory.\n");
                		else
                    			printf ("\n250 Directory successfully changed.\n"); 
				
            		}
			printf("\n\n---------------------------------------------------------------\n");
        	}

		else if(strcmp("RMD",command)==0)
		{
			send(sockfd,command,100,0);
			printf("\n\nEnter name of directory u want to remove : ");
            		scanf("%s", arg);
            		printf("(RMD):%s\n", arg);

            		if (rmdir(arg) < 0)
                		printf("\n550 Failed to remove directory.\n");
            		else
                		printf ("\n250 Directory successfully removed.\n"); 

			printf("\n\n---------------------------------------------------------------\n");
        	}
      		
		else if(strcmp("RETR",command)==0)
		{
			send(sockfd,command,100,0);
			if(login == 2){
      			printf("\n\nEnter file name to get from server: ");
            		scanf("%s", arg);

			send(sockfd,arg,100,0);
			int size;
			recv(sockfd, &size, sizeof(int), 0);
			

            		if (!size){
                		printf("\n450 Requested file action not taken. File unavaible.\n");
                		//close(data_socket);
            		}

            		else{

                		char buffer[100];
				FILE *fp;
				int ch = 0;
				fp = fopen("new_file.txt","a");
				int words;
				read(sockfd,&words,sizeof(int));
				while(ch!=words)
				{
					read(sockfd,buffer,512);
					fprintf(fp," %s",buffer);
					ch++;
				}
				printf("\nThe new file created is new_file.txt...");
				printf("\nThe file was received successfully...\n");
           		}
			}
			printf("\n\n---------------------------------------------------------------\n");
        	}    
		
		else if(strcmp("STOR",command)==0)
		{
			
			send(sockfd,command,100,0);
			if(login == 2){
			char buffer[100];
			char filename[100];
			FILE *f;
			printf("\n\nEnter file name to store on server: ");
            		scanf("%s", filename);
    
    			int words = 0;
    			char c;
     			f=fopen(filename,"r");
			if(!f)
			{
				printf("\n450 Requested file action not taken. File unavaible.\n");
			}
			else
			{

		    		while((c=getc(f))!=EOF)			//Counting No of words in the file
				{	
					fscanf(f , "%s" , buffer);
					if(isspace(c)||c=='\t')
						words++;	
				}
			//printf("Words = %d \n"  , words);	
       
      
				write(sockfd, &words, sizeof(int));
     				rewind(f);
      
            	
       
    				char ch ;
       				while(ch != EOF)
      				{
		
					fscanf(f , "%s" , buffer);
					//printf("%s\n" , buffer);	
					write(sockfd,buffer,512);
					ch = fgetc(f);
      				}
				printf("\nThe file was sent successfully\n");
			}
			}
			printf("\n\n---------------------------------------------------------------\n");
    
   	     	}
		else if(strcmp("ABOR",command)==0)
		{
			send(sockfd,command,100,0);
			printf("\nClosing Connection...");
			passive = 1;
			close(connfd);
			printf("\n\n---------------------------------------------------------------\n");
		}
		else if(strcmp("LIST",command)==0)
		{
			send(sockfd,command,100,0);
			if(login == 2)
			{
			
			printf("\nDisplaying list of files...\n");
			char buffer[1000];
			//char list_of_files[1000];
			recv(sockfd,buffer,1000,0);
			char *p = strtok(buffer," ");
			while(p!=0){
				cout<<p<<'\n';
				p = strtok(NULL," ");
			}
			printf("\nFiles received...");
			}
			printf("\n\n---------------------------------------------------------------\n");
			
		}

	
		else if(strcmp("QUIT",command)==0)
		{
			send(sockfd,command,100,0);
			if(login == 2){
			printf("\n221 Service closing control connection.\n");
			close(connfd);
			close(sockfd);
			ch = true;
			}
			printf("\n\n---------------------------------------------------------------\n");
		}
		else
		{
			printf("\n503 Bad sequence of commands....\n");
			
		}
		

	}
		

	
}


