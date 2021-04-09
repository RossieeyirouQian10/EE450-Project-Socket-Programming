//
//  Admission.cpp
//  ProjectPhase2
//
//  Created by 钱依柔 on 7/4/19.
//  Copyright © 2019 钱依柔. All rights reserved.
//

#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>

/*
 USC ID: 4354716429
 DepartmentA 1 TCP (phase 1), 1 UDP 21100 + 429 (phase 2)
 DepartmentB 1 TCP (phase 1), 1 UDP 21200 + 429 (phase 2)
 DepartmentC 1 TCP (phase 1), 1 UDP 21300 + 429 (phase 2)
 Admission Max 8 UDP (phase 2), 1 TCP 3300 + 429 (phase 1 and phase 2)
 student1 1 TCP (phase 2), 1 UDP 21400 + 429 (phase 2)
 student2 1 TCP (phase 2), 1 UDP 21500 + 429 (phase 2)
 student3 1 TCP (phase 2), 1 UDP 21600 + 429 (phase 2)
 Student4 1 TCP (phase 2), 1 UDP 21700 + 429 (phase 2)
 Student5 1 TCP (phase 2), 1 UDP 21800 + 429 (phase 2)
 */

#define Admission_TCP_PORT 3729 // 3300 + 429 (USC ID)
#define department_save_file "department.txt"

#define ShowDbg 1

const short g_department_port[3] = {21100, 21200, 21300};
const short g_student_port[5] = {21400, 21500, 21600, 21700, 21800};
int g_nUdpPort = 21900;

using namespace std;

int udp_sockfd;
int tcp_sockfd;
map<string, float> g_departmentInfo;

// get sockaddr, IPv4 or IPv6
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A simple Stream Server"
//From Lab2
//From Project Phase 1
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

uint16_t get_in_addr_port(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return (((struct sockaddr_in*)sa)->sin_port);
    }
    
    return (((struct sockaddr_in6*)sa)->sin6_port);
}

//wait
void waitAllSubPro(int* pid, int nNum){
    int status;
    pid_t ret;
    for(int i = 0; i < nNum; i++){
        do{
            usleep(100);
            ret = waitpid(pid[i], &status, WNOHANG);
        }while (ret == 0);
    }
}

//split information
//From Lab4
vector<string> splitStr(const string& strSrc, const string& strSplit){
    vector<string> vsRet;
    string::size_type pos1, pos2;
    pos2 = strSrc.find(strSplit);
    pos1 = 0;
    while(string::npos != pos2){
        vsRet.push_back(strSrc.substr(pos1, pos2-pos1));
        
        pos1 = pos2 + strSplit.size();
        pos2 = strSrc.find(strSplit, pos1);
    }
    if(pos1 != strSrc.length()){
        vsRet.push_back(strSrc.substr(pos1));
    }
    return vsRet;
}

//deal with GPA number
float Str2Float(string strValue){
    float fRet = 0.00f;
    sscanf(strValue.c_str(), "%f", &fRet);
    return fRet;
}

//TCP server
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A simple Stream Server"
//From Lab2
//From Project Phase 1
void tcp_server(int port){
    struct hostent *he;
    
    tcp_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //define scoket
    if(tcp_sockfd < 0){
        perror("ERROR opening socket");
    }
    
    struct sockaddr_in servaddr;
    memset((char *) &servaddr, 0, sizeof(servaddr)); // make sure the struct is empty
    //set bind information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    //get hostname
    if((he = gethostbyname("nunki.usc.edu")) == NULL){
        perror("gethostbyname");
    }
    //bind
    if(bind(tcp_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("ERROR on binding");
    }
    printf("The admission office has TCP port %d and IP address %s\n", port, inet_ntoa(*(struct in_addr*)he->h_addr));//print message
    if(listen(tcp_sockfd, 5) < 0){
        perror("ERROR on listening");
    }
}

/* Phase 2 */
//UDP socket
void create_udp_socket(){
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_sockfd == -1){
        perror("socket");
    }
    
    //The adminssion office has UDP port ... and IP adddress ... for Phase2
    struct sockaddr_in udp_servaddr;
    memset(&udp_servaddr, 0, sizeof(udp_servaddr));
    
    udp_servaddr.sin_family = AF_INET;
    udp_servaddr.sin_addr.s_addr = INADDR_ANY;
    udp_servaddr.sin_port = htons(g_nUdpPort);
    if (bind(udp_sockfd, (const struct sockaddr *)&udp_servaddr, sizeof(udp_servaddr)) < 0) perror("bind failed");
    
    
    //<S#> has UDP port ... and IP address ... for Phase 2
    struct hostent *he;
    if ((he = gethostbyname("nunki.usc.edu")) == NULL){
        perror("gethostbyname");
    }
    printf("The adminssion office has UDP port %d and IP address %s for Phase2\n", g_nUdpPort, inet_ntoa(*(struct in_addr*)he->h_addr));
}

//clear file information
//From Project Phase 1
void clear_file_info(string filename){
    ofstream ofs;
    ofs.open(filename.c_str(), ofstream::out | ofstream::trunc);
    ofs.close();
}

//store information of department
//From Lab1 and Project Phase 1
string store_department_info(string department_info){
    string department_name = department_info.substr(0,11);    //department name: from 0-10
    ofstream res;    //rewrite the information to lines in file
    res.open(department_save_file, ios::out | ios::app);
    res << department_info;
    //    res << endl;
    res.close();
    
    return department_name;
}

//read information of department
void read_department_info(){
    ifstream infile(department_save_file);    //open file
    string line;
    //read line
    while(getline(infile, line)){
        if(line.length() <= 0){
            continue;
        }
        
        vector<string> vsAll = splitStr(line, ",");
        if(vsAll.size() < 2){
            continue;
        }
        vector<string> vsNameAndScore = splitStr(vsAll[1], "#");
        if(vsNameAndScore.size() < 2){
            continue;
        }
        g_departmentInfo[vsNameAndScore[0]] = Str2Float(vsNameAndScore[1]);
#if ShowDbg
        printf("read_department_info->%s:%s\n", vsNameAndScore[0].c_str(), vsNameAndScore[1].c_str());
#endif
    }
}

//receive message from department
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A simple Stream Server"
//From Lab2
//From Project Phase 1
void receive_from_department(int numOfClients){
    int newsockfd;
    char buffer[256] = {0};
    int numbytes;
    struct sockaddr_in cliAddr;
    socklen_t cli_len = sizeof(cliAddr);
    int* pid = new int[numOfClients];
    
    for(int ct = 0; ct < numOfClients; ct++){
        newsockfd = accept(tcp_sockfd, (struct sockaddr *) &cliAddr, &cli_len);    //get connection
        if(newsockfd < 0){
            perror("ERROR on accept");
        }
        
        pid[ct] = fork();
        if(pid[ct] < 0){
            perror("ERROR on fork");
        }
        if(pid[ct] == 0){
            //子进程处理逻辑
            close(tcp_sockfd);
            numbytes = read(newsockfd, buffer, 255);    //read message from department
            while(numbytes > 0){
                string department = store_department_info(string(buffer));    //read information into files
                printf("Received the program list from <%s>\n", department.c_str());    //print message
                memset(buffer,0,256);
                numbytes = read(newsockfd, buffer, 255);    //read message from department
            }
            //if (numbytes < 0){
                 //perror("ERROR reading from socket");
            //}
            exit(0);
        }else{
            close(newsockfd);
        }
    }
    waitAllSubPro(pid, numOfClients);
    delete[] pid;
    //sleep(1);    //wait
    cout << "End of Phase I part 1 for the admission office" << endl;
}

//receive message from student
void receive_from_student(int numOfClients){
    int newsockfd;
    char buffer[256] = {0};
    int numbytes;
    struct sockaddr_in cliAddr;
    socklen_t cli_len = sizeof(cliAddr);
    int* pid = new int[numOfClients];
    
    for(int ct = 0; ct < numOfClients; ct++){
        newsockfd = accept(tcp_sockfd, (struct sockaddr *) &cliAddr, &cli_len);    //get connection
        if(newsockfd < 0){
            perror("ERROR on accept");
        }
        
        pid[ct] = fork();
        if(pid[ct] < 0){
            perror("ERROR on fork");
        }
        if(pid[ct] == 0){
            //child socket
            close(tcp_sockfd);
            numbytes = read(newsockfd, buffer, 255);    //read message from department
            //student3,GPA:3.4;Interest1:B1;Interest2:C2;Interest3:C3
            vector<string> vsAll = splitStr(buffer, ",");
            if(vsAll.size() < 2){
                perror("ERROR reading from socket");
                exit(0);
            }
            string strName = vsAll[0];
            int nStuent = strName.at(7) - '0';
            
            printf("Adminssion office receive the application from <%s>\n", strName.c_str());    //print message
            vector<string> vsInfo = splitStr(vsAll[1], ";");
            if(vsInfo.size() < 2){
                perror("ERROR reading from socket");
                exit(0);
            }
            
            vector<string> vsGPAInfo = splitStr(vsInfo[0], ":");
            if(vsGPAInfo.size() < 2){
                perror("ERROR reading from socket");
                exit(0);
            }
            
            string strGPA = vsGPAInfo[1];
            float fGPA = Str2Float(strGPA);
            
            bool bHaveRightInsterest = false;
            vector<string> vsInterest;
            for(int i = 1; i < vsInfo.size(); i++){
                vector<string> vsInterestInfo = splitStr(vsInfo[i], ":");
                if(vsInterestInfo.size() < 2){
                    continue;
                }
#if ShowDbg
                printf("receive_from_student->%s:%s\n", vsInterestInfo[0].c_str(), vsInterestInfo[1].c_str());
#endif
                if(!bHaveRightInsterest && g_departmentInfo.find(vsInterestInfo[1]) != g_departmentInfo.end()){
                    bHaveRightInsterest = true;
                }
                vsInterest.push_back(vsInterestInfo[1]);
            }
            
            struct sockaddr_in udp_addr_student;
            memset(&udp_addr_student, 0, sizeof(udp_addr_student));
            udp_addr_student.sin_family = AF_INET;
            udp_addr_student.sin_port = htons(g_student_port[nStuent-1]);
            udp_addr_student.sin_addr.s_addr = cliAddr.sin_addr.s_addr;
            
            int nSendInt = bHaveRightInsterest ? 1 : 0;
            send(newsockfd, &nSendInt, sizeof(int), 0);
            sleep(1);
            if(bHaveRightInsterest){
                string strInterest;
                bool bAccept = false;
                for(int i = 0; i < vsInterest.size(); i++){
                    map<string, float>::iterator it = g_departmentInfo.find(vsInterest[i]);
                    if(it == g_departmentInfo.end()){
                        continue;    //no target
                    }
                    if(it->second > fGPA){
                        continue;    //GPA lower the standard
                    }
                    
                    bAccept = true;
                    //Accept#A1#departmentA
                    //The adminssion office has send the application result to <Student#>
                    string strSendStudent = "Accept#" + it->first + "#department" + it->first.at(0);
                    sendto(udp_sockfd, strSendStudent.c_str(), strSendStudent.length(), 0, (const struct sockaddr *) &udp_addr_student, sizeof(udp_addr_student));
                    printf("The adminssion office has send the application result to <%s>\n", strName.c_str());    //print message
                    
                    //e.g. Student1#3.8#A1
                    //The adminssion office has send one admitted student to <D#>
                    string strSendDepartment = strName + "#" + strGPA + "#" + it->first;
                    int nDepartment = it->first.at(0) - 'A';
                    struct sockaddr_in udp_addr_department;
                    memcpy(&udp_addr_department, &cliAddr, sizeof(udp_addr_department));
                    udp_addr_department.sin_port = htons(g_department_port[nDepartment]);
                    sendto(udp_sockfd, strSendDepartment.c_str(), strSendDepartment.length(), 0, (const struct sockaddr *) &udp_addr_department, sizeof(udp_addr_department));
                    printf("The adminssion office has send one admitted student to <Department%c>\n", it->first.at(0));    //print message
                    
                    break; //only admit one student
                }
                if(!bAccept){
                    //Reject
                    //The adminssion office has send the application result to <Student#>
                    string strSendStudent = "Reject";
                    sendto(udp_sockfd, strSendStudent.c_str(), strSendStudent.length(), 0, (const struct sockaddr *) &udp_addr_student, sizeof(udp_addr_student));
                    printf("The adminssion office has send the application result to <%s>\n", strName.c_str());    //print message
                }
            }
            exit(0);
        }else{
            close(newsockfd);
        }
    }
    waitAllSubPro(pid, numOfClients);
    delete[] pid;
    //sleep(1);    //wait
    
    cout << "End of Phase 2 for the admission office" << endl;
}

//main function
int main(int argc, char *argv[]){
    clear_file_info(department_save_file);
    tcp_server(Admission_TCP_PORT);    //bind and listen
    receive_from_department(3);    //receive message from department
    read_department_info();        //read department information
    create_udp_socket();
    receive_from_student(5) ;    //receive message from student
    return 0;
}
