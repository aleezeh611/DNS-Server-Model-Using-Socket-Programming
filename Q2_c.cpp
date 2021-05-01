// udp client driver program 
#include"Q2_utility.h"
#include <pthread.h>
#include<mutex>
using namespace std;

#define PORT 5000 
#define MAXLINE 1000 

	
int main(){
	char buffer[2000]; 
	int sockfd, n; 
    char ch_mess[100];
	char startupmessage[20];
    string mess;
	struct sockaddr_in servaddr; 

    // clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
	
	// create datagram socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	
	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 
		printf("\n Error : Connect Failed \n"); 
		exit(0); 
	} 

	char status[5] = "1";					//initially will run normally

	while(true){    
		printf("REQUEST FOR SERVER STATUS: \n");
		fgets(startupmessage, 20, stdin);
		sendto(sockfd, startupmessage, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		recvfrom(sockfd, status, sizeof(status), 0, (struct sockaddr*)NULL, NULL);		//GET STATUS I.E NORMAL RUNNING OR ALERTS
		cout<<status<<endl;

		if(strcmp(status,"1") == 0){
			printf("Choose one of the following actions:\n1. Enter INVOICE NUMBER to search attribute/delete/update\n2. Enter '-' to add another item/get serverlog/exit \n");
	    	getline(cin,mess, '\n');
    		StringtoChar(mess, ch_mess);
    		sendto(sockfd, ch_mess, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
			printf("Choose one of the following(use all small letters):\n1. Enter attribute you would like to search\n2. Enter 'delete' to delete record\n3. Enter 'update' to update record\n4. Enter 'add' to add anotehr item\n5. Enter 'serverlog' to get the serverlog\n6. Enter 'exit' to exit the program\n");
			getline(cin,mess, '\n');
	    	StringtoChar(mess, ch_mess);
		    sendto(sockfd, ch_mess, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		
			if(strcmp(ch_mess,"exit") == 0) {
                // waiting for response 
			    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
			    puts(buffer);
                break;
            }
            if(strcmp(ch_mess, "update") == 0){
                for(int i = 0 ; i < 3; i++){
                    cout<<i<<endl;
                    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);		//GET STATUS I.E NORMAL RUNNING OR ALERTS
                    puts(buffer);
                    getline(cin,mess, '\n');
	        	    StringtoChar(mess, ch_mess);
		            sendto(sockfd, ch_mess, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
                }
            }
			printf("\n");
    		
			// waiting for response 
			recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
			puts(buffer);
		}
		else if(strcmp(status,"2") == 0){
			cout<<"status is correct\n";
            recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
			puts(buffer);
            if(strcmp(buffer,"\n ALERT :: AUTHORIZATION REQUIRED TO DELETE INVOICE") == 0){
				fgets(ch_mess,100,stdin);
				cout<<ch_mess<<endl;
                while(strncmp(ch_mess, "Y",1) != 0 && strncmp(ch_mess,"N",1) != 0){
					printf("ERROR: INVALID INPUT PLEASE ENTER [Y/N] FOR AUTHORIZATION\n" );
					fgets(ch_mess,100,stdin);
				}
			sendto(sockfd, ch_mess , MAXLINE, 0, (struct sockaddr*)NULL,sizeof(sockaddr_in)); 
			}
		}
		else if(strcmp(status,"3") == 0){
            recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);		//GET STATUS I.E NORMAL RUNNING OR ALERTS
		    cout<<buffer<<endl;
        }
        printf("-----------------------------\n\n"); 
    }
	// close the descriptor 
	close(sockfd); 

}