#include"Q2_DataStructures.h"
using namespace std;

void ReadFile(string filename, InvList& invoicelist){
    string line;
    ifstream file;
    file.open(filename);

    if(file.is_open()){
        for(int i = 0 ; i < 2; i++) getline(file,line);

        string num,desc;
        string q;
        string p;
        string cid,cntry;
        while(getline(file,num,'\t')){
            getline(file,desc,'\t');
            getline(file, q,'\t');
            getline(file,p,'\t');
            getline(file,cid,'\t');
            getline(file,cntry,'\n');
            invoicelist.Insert(num,desc,q,p,cid,cntry);
        }
        file.close();
    }
    else cout<<"-- ERROR:: File could not be opened -- \n";
}

bool RemoveInvoice(InvList& invoicelist, string invnum){
    bool removed = false;
    
    while(invoicelist.Remove(invnum)){
        removed = true;
    }
    return removed;
}

void getAttribute(string attr, InvList invoicelist, string invnum){
    invoicelist.displayAttribute(invnum, attr);
}

void StringtoChar(string str, char ch[100]){
    int i = 0;
    for(; str[i] != '\0'; i++){
        ch[i] = str[i];
    }
    ch[i] = '\0';
    cout<<endl;
}

string getDate(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
   
    string time_ = to_string(ltm->tm_mday);
    string s;

    s.append(time_);
    s.append("/");
    time_ = to_string(ltm->tm_mon+1);
    s.append(time_);
    s.append("/");
    time_ = to_string(ltm->tm_year+1900);
    s.append(time_);
    return s;
}
string getTime(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
   
    string time_ = to_string(ltm->tm_hour);
    string s;
    s.append(time_);
    s.append(":");
    time_ = to_string(ltm->tm_min);
    s.append(time_);
    s.append(":");
    time_ = to_string(ltm->tm_sec);
    s.append(time_);
    return s;
}

void AddRecordToServerLog(sockaddr_in cl, string filename, string invnum, string action){
    //reading lines for serial numbers
    int count = 0;
    string line;
    ifstream lineread(filename);
    while (getline(lineread, line)) count++;
    
    //writing to file
    ofstream file(filename, ios::app);
    if(file.is_open()){
        
        file<<count+1<<".\t"<<ntohs(cl.sin_port)<<"\t"<<getTime()<<"\t"<<getDate();
        file<<"\t"<<invnum<<"\t"<<action<<endl;
    
        file.close();
    }
    else{
        cout<<"ERROR:: FILE NOT OPEN\n";
    }
}

void Tokenize(char * str, char tokens[3][10], int* n){
	int c = 0;
	int j = 0;
	for(int i = 0; str[i] != '\0' ; i++){
		if(str[i]!=' ' && str[i] != '\t' && str[i] != '\n' ){
			tokens[c][j] = str[i];
			j++;
		}
		else if(!( (str[i] == ' ' || str[i] == '\t') && str[i+1] == '\0')){
			tokens[c][j] = '\0';
			j = 0;
			c++;
		}
	}
	tokens[c][j] = '\0';
	*n = c;
}