#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<sys/wait.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <arpa/inet.h> 
#include <ctime>
using namespace std;

#ifndef null
#define  null 0
#endif

struct ClientNode{
    struct sockaddr_in clientaddr;
    ClientNode* next;
    ClientNode(){
        next = NULL;
    }
    bool initializenext(){
        if(next==NULL){
            next = new ClientNode;
            return true;
        }
        else return false;
    }
};

class ClientList{
    public:
    ClientNode* head;

    ClientList(){
        head = NULL;
    }

    bool AddItem(sockaddr_in cl){
        if(head == NULL){
            head = new ClientNode;
            head->clientaddr = cl;
            return true;
        }
        else{
            ClientNode* temp = head;
            bool alreadyexists = false;
            while(temp != NULL){
                if(temp->clientaddr.sin_port == cl.sin_port){             //look for the client addr to see if already stored in linked list
                    alreadyexists = true;               //do not add again if already there
                    break;
                }
                if(temp->next == NULL) break;           //otherwise if reached end of linked list then break loop and add it
                temp = temp->next;
            }
            if(!alreadyexists){
                temp->initializenext();
                temp = temp->next;
                temp->clientaddr = cl;
                return true;
            }
            else return false;
        }
    }

    void DisplayPortNumbers(){
        ClientNode* temp = head;
        while(temp != NULL){
            printf("Port Number:  %d \n",ntohs(temp->clientaddr.sin_port));
            temp = temp->next;
        }
    }

    bool IsEmpty(){
        if(head == NULL) return true;
        else return false;
    }

    ~ClientList(){
        ClientNode* temp;
        while(head!=NULL){
            temp = head;
            head = head->next;
            delete temp;
        }
    }
};

struct Invoice_{
    string invoice_num;
    string description;
    string quantity;
    string price;
    string customer_id;
    string country;
    
    void EnterData(string invnum, string desc, string qnty, string pr, string custid, string cntry){
        invoice_num = invnum;
        description = desc;
        quantity = qnty;
        price = pr;
        customer_id = custid;
        country = cntry;
    }
};

struct InvNode{
    InvNode* next;
    Invoice_ Inv;
    
    InvNode(){
        next = null;
    }
    bool InitializeNext(){
        if(next == NULL){
            next = new InvNode;
            return true;
        } 
        else return false;
    }
    void EnterData(string invnum, string desc, string qnty, string pr, string custid, string country){
       Inv.EnterData(invnum, desc, qnty, pr, custid, country) ;
    }
    void Display(){
        cout<<Inv.invoice_num<<"\t"<<Inv.description<<"\t"<<Inv.quantity<<"\t"<<Inv.price<<"\t"<<Inv.customer_id<<"\t"<<Inv.country<<"\n";
    }
    string SendAllAttributes(){
        string toReturn;
        toReturn.append(Inv.invoice_num);
        toReturn.append("\t");
        toReturn.append(Inv.description);
        toReturn.append("\t");
        toReturn.append(Inv.quantity);
        toReturn.append("\t");
        toReturn.append(Inv.price);
        toReturn.append("\t");
        toReturn.append(Inv.customer_id);
        toReturn.append("\t");
        toReturn.append(Inv.country);
        return toReturn;
    }
    string displayAttribute(string attr){
        if(attr == "description") return Inv.description;
        else if(attr == "quantity") return Inv.quantity;
        else if(attr == "price") return Inv.price;
        else if(attr == "customer id") return Inv.customer_id;
        else if(attr == "country") return Inv.country;
        else if(attr == "all") return SendAllAttributes();
        else{
            string returnerror = " -- ERROR INVALID INPUT -- \n";
            return returnerror;
        }
    }
};

class InvList{
	InvNode* head;
public:
	InvList() {
		head = NULL;
	}

	void Insert(string invnum, string desc, string qnty, string pr, string custid, string country) {
		if (head == NULL) {					//incase list previously empty
			head = new InvNode;
			head->EnterData(invnum, desc, qnty, pr, custid, country);
		}
		else if (head != NULL) {			//not empty list so insertion at end
			InvNode* temp = head;
            while(temp->next!=NULL) temp = temp->next;
			if (temp->next == NULL) {
				temp->InitializeNext();
				temp->next->EnterData(invnum, desc, qnty, pr, custid, country);
			}
		}
	}

	bool Remove(string invnum) {
        if(!isEmpty()){
            InvNode* temp = head;
            if(head->Inv.invoice_num == invnum){    //if we want to delete head
                temp = head;
                head = head->next;
                delete temp;
                return true;
            }

            while(temp->next != NULL) {
                if(temp->next->Inv.invoice_num == invnum) break;
                else    temp = temp->next;
            }
            if(temp->next != NULL){
            	if (temp == head ) {					//if we want to delete the element right after head
    	            temp = head->next;
                    head->next = head->next->next;
                    delete temp;
    		    }
	    	    else {
                    InvNode* toDelete = temp->next;
                    temp->next = temp->next->next;
                    delete toDelete;            
                }
                return true;
            }
            return false;
	    }
        else{
            cout<<"List is empty\n";
            return false;
        }
    }

    bool UpdateItem(string invnum, string attr, string newattr){
        bool updated = false;
        InvNode* temp = head;
        while(temp!=NULL){
            if(temp->Inv.invoice_num == invnum){
                if(attr == "customer id") temp->Inv.customer_id = newattr;
                else if(attr == "description") temp->Inv.description = newattr;
                else if(attr == "quantity") temp->Inv.quantity = newattr;
                else if(attr == "price") temp->Inv.price = newattr;
                else if(attr == "country") temp->Inv.country = newattr;
                else {
                    cout<<"INVALID ATTRIBUTE GIVEN\n";
                    return false;
                }
                updated = true;
            }
            temp = temp->next;
        }
        return updated;
    }
    
	bool isEmpty() {
		if (head == NULL) return true;
		else return false;
	}

	void display() {
		InvNode* temp = head;
		if (temp == NULL) cout << "ERROR: LIST IS EMPTY\n";
		else {
			while (temp != NULL) {
				temp->Display();
				temp = temp->next;
			}
		}
	}

    string displayAttribute(string invnum, string attr){
        InvNode* temp = head;
        string toReturn;
        bool found = false;
        while(temp != NULL){
            if(temp->Inv.invoice_num != invnum ) temp = temp->next;
            else{
                toReturn.append( temp->displayAttribute(attr));
                toReturn.append("\n");
                found = true;
                temp = temp->next;
            }
        }
        if(found == false){
            toReturn = "ERROR :: RECORD DOES NOT EXIST\n";
        }
        return toReturn;
    }

	~InvList() {
		InvNode* temp;
        while(head!=NULL){
            temp = head;
            head = head->next;
            delete temp;
        }
	}
};

