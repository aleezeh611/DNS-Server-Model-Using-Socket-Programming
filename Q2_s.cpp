#include"Q2_utility.h"
#define PORT 5000 
#define MAXLINE 2000 

int main(){
    //first create database from files
    InvList invoicelist;
    ReadFile("Assignment 1.txt", invoicelist);    
	ClientList cl_list;
  
    //now set up UDP socket
	char buffer[100]; 
	char message[2000]; 
	char ignoremessage[10] = "ignore";
	string tempstr;
	int listenfd, len; 
	struct sockaddr_in servaddr, cliaddr;
	bzero(&servaddr, sizeof(servaddr)); 

	// Create a UDP Socket 
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);		 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 

	// bind server address to socket descriptor 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	int i = 0;
	char status[5] = "1";						//initially be normal
	//1 -> continue normally
	//2 -> delete so authorization message
	//3 -> alert only


    do{
		//receive the datagram 
		len = sizeof(cliaddr); 
		int n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
		buffer[n] = '\0'; 
		sendto(listenfd, status, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		printf("Port Number:  %d REQUESTING ACTION\n",ntohs(cliaddr.sin_port));
		if(cl_list.AddItem(cliaddr)) i++;
				
		len = sizeof(cliaddr); 
		n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
		buffer[n] = '\0'; 
		string invnum;
		invnum.append(buffer);
		n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
		buffer[n] = '\0'; 
		string attribute;
		attribute.append(buffer);
		
		printf("Port Number:  %d \n\n",ntohs(cliaddr.sin_port));
		puts(buffer);
		
		if(strcmp(buffer,"exit") != 0) AddRecordToServerLog(cliaddr, "ServerLog.txt", invnum, attribute);

		if(strcmp(buffer,"exit") == 0){
			i--;
			tempstr = "GOOD BYE CLIENT";
			StringtoChar(tempstr,message);
			status[0] = '1';
		}
		else if(strcmp(buffer, "delete") != 0){
			if(strcmp(buffer, "all") == 0 || strcmp(buffer, "description") == 0 ||strcmp(buffer, "quantity") == 0 ||strcmp(buffer, "price") == 0 ||strcmp(buffer, "customer id") == 0 ||strcmp(buffer, "country") == 0)
			{
				StringtoChar(invoicelist.displayAttribute(invnum,attribute),message);
				status[0] = '1';
			}
			else if(strcmp(buffer, "update") == 0){
				string inum, attr, newattr;
				tempstr = "ENTER INVOICE TO CHANGE: ";
				StringtoChar(tempstr,message);
				sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
				n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
				buffer[n] = '\0';
				inum.append(buffer);
				tempstr = "ENTER ATTRIBUTE TO UPDATE: ";
				StringtoChar(tempstr,message);
				sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
				n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
				buffer[n] = '\0';
				attr.append(buffer);
				tempstr = "ENTER UPDATED VALUE: ";
				StringtoChar(tempstr,message);
				sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
				n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receive message from server 
				buffer[n] = '\0';
				newattr.append(buffer);
				
				if(invoicelist.UpdateItem(inum,attr,newattr)) {
					tempstr = "INVOICE HAS BEEN SUCCESSFULLY UPDATED";
				}
				else{
					tempstr = "ERROR :: MESSAGE COULD NOT BE UPDATED";
				}
				StringtoChar(tempstr,message);

				status[0] = '3';
				ClientNode* temp = cl_list.head ;
				char sendalert1[15] = "INVOICE # ";
				string sendalert2;
				sendalert2.append(sendalert1);
				sendalert2.append(invnum);
				char sendalert3[20] = "HAS BEEN UPDATED\n";
				sendalert2.append(sendalert3);
				StringtoChar(sendalert2, sendalert1);
				while(temp!=NULL){
					if(temp->clientaddr.sin_port != cliaddr.sin_port){
						sendto(listenfd, status, MAXLINE, 0, (struct sockaddr*)&temp->clientaddr, sizeof(temp->clientaddr)); 
						sendto(listenfd, sendalert1, MAXLINE, 0, (struct sockaddr*)&temp->clientaddr, sizeof(temp->clientaddr)); 
					}
				temp = temp->next;
				}
			}
		}
		else{
			status[0] = '2';
			ClientNode* temp = cl_list.head ;
			char askotherclients[100] = "\n ALERT :: AUTHORIZATION REQUIRED TO DELETE INVOICE";
			bool authorized = true;
			int count=0;
			while(temp!=NULL){
				if(temp->clientaddr.sin_port != cliaddr.sin_port){
					sendto(listenfd, status, MAXLINE, 0, (struct sockaddr*)&temp->clientaddr, sizeof(temp->clientaddr)); 
					sendto(listenfd, askotherclients, MAXLINE, 0, (struct sockaddr*)&temp->clientaddr, sizeof(temp->clientaddr)); 
					n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&temp->clientaddr,(socklen_t*)&len); //receive message from server 
					while(strncmp(buffer,"N",1)!=0 && strncmp(buffer,"Y",1)!=0){
						n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&temp->clientaddr,(socklen_t*)&len); //receive message from server 
						buffer[n] = '\0'; 
						if(strncmp(buffer,"N",1)==0)authorized = false;
					}			
				}
				temp = temp->next;
			}
			if(authorized){
				if(RemoveInvoice(invoicelist,invnum))tempstr = "RECORD HAS BEEN DELETED\n";
				else tempstr = "ERROR : RECORD COULD NOT BE DELETED\n";
			}
			else tempstr = "ERROR : AUTHORIZATION NOT GRANTED\n";
			StringtoChar(tempstr, message);
		}
		// send the response 
		sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		status[0] = '1';
		
	}while(i>0);

	cout<<"\n ---------------------------------------------------------------\nFINAL INVOICE FILE: \n\n";
	invoicelist.display();

    return 0;
}
