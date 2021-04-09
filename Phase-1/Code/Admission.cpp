//
//  Admission.cpp
//  projectPhase1
//
//  Created by 钱依柔 on 6/15/19.
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

using namespace std;

int tcp_sockfd;

//TCP server
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A simple Stream Server"
//From Lab2
void tcp_server(int port){
    struct hostent *he;
    
    tcp_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    //define scoket
    if (tcp_sockfd < 0){
        perror("ERROR opening socket");
    }
    
    struct sockaddr_in servaddr;
    memset((char *) &servaddr, 0, sizeof(servaddr)); // make sure the struct is empty
    //set bind information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    //get hostname
    if ((he = gethostbyname("viterbi-scf2.usc.edu")) == NULL){
        perror("gethostbyname");
    }
    //bind
    if (bind(tcp_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("ERROR on binding");
    }
    printf("The admission office has TCP port %d and IP address %s\n", port, inet_ntoa(*(struct in_addr*)he->h_addr));//print information
    if (listen(tcp_sockfd, 5) < 0){
        perror("ERROR on listening");
    }
}

//clear the file information
void clear_file_info(string filename){
    ofstream ofs;
    ofs.open(filename.c_str(), ofstream::out | ofstream::trunc);
    ofs.close();
}

//store the information from department
//From Lab1
string store_department_info(string department_info){
    string department_name = department_info.substr(0,11);    //total number of dapartment name is 11
    ofstream res;    //output the data of the file saved from department
    res.open(department_save_file, ios::out | ios::app);
    res << department_info;
    //res << endl;
    res.close();
    
    return department_name;
}

//receive information from department
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A simple Stream Server"
//From Lab2
void receive_from_department(int numOfClients){
    int newsockfd, pid;
    char buffer[256];
    int numbytes;
    struct sockaddr_in cliAddr;
    socklen_t cli_len = sizeof(cliAddr);
    
    for(int ct = 0; ct < numOfClients; ct++){
        newsockfd = accept(tcp_sockfd, (struct sockaddr *) &cliAddr, &cli_len); //receive connection
        if (newsockfd < 0){
            perror("ERROR on accept");
        }
        
        pid = fork();
        if (pid < 0){
            perror("ERROR on fork");
        }
        if (pid == 0){
            //child socket dealing with message
            close(tcp_sockfd);
            numbytes = read(newsockfd, buffer, 255);    //read the message from department
            while (numbytes > 0){
                string department = store_department_info(string(buffer));    //read in message and store it
                printf("Received the program list from <%s>\n", department.c_str());    //print information
                numbytes = read(newsockfd, buffer, 255);    //read the message from department
            }
            //if (numbytes < 0) perror("ERROR reading from socket"); 
            exit(0);
        }else{
            close(newsockfd);
        }
    }
    sleep(1);    //delay
    cout << "End of Phase I part 1 for the admission office" << endl;
}

int main(int argc, char *argv[]){
    clear_file_info(department_save_file);
    tcp_server(Admission_TCP_PORT);    //bind and listen
    receive_from_department(3);    //receive message from department
    return 0;
}

