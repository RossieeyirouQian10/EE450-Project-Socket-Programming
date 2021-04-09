//
//  Department.cpp
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
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>

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

#define Admission_TCP_PORT "3729" // 3300 + 429 (USC ID)
#define HOSTNAME "viterbi-scf2.usc.edu"
//#define HOSTNAME "localhost"

const short g_department_port[3] = {21100, 21200, 21300};

using namespace std;

int udp_sockfd;
int tcp_sockfd;

// get sockaddr, IPv4 or IPv6
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.2. A simple Stream Client"
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

//read file and send them
//From Lab1
//From Project Phase 1
void readFileAndSend(string department_name){
    //string res = "";
    string filename = department_name + ".txt";    //file name
    ifstream infile(filename.c_str());    //open file
    string line, data;
    //read line
    int numbyte;
    while (getline(infile, line)){
        if(line.length() > 0){
            data = department_name + "," + line + "\n";
            //send data
            if((numbyte = send(tcp_sockfd, data.c_str(), data.length(), 0)) > 0){
                printf("<%s> has sent <%s,%s> to the admission office\n", department_name.c_str(), department_name.c_str(), line.c_str());
            }
        }
    }
}

//connect to admission
//From Beej's Guide to C Programming and Network Programming (socket programming):"6.2. A simple Stream Client"
//From Lab2
//From Project Phase 1
int connect_to_admission(string department_name){
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN] = {0};
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if((rv = getaddrinfo(HOSTNAME, Admission_TCP_PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    //loop traverse all the results and connect to the first found result
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((tcp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if(connect(tcp_sockfd, p->ai_addr, p->ai_addrlen) == -1){
            //perror("client: connect");
            close(tcp_sockfd);
            continue;
        }
        break;
    }
    
    if(p == NULL){
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    //inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    
    struct sockaddr tcp_cliaddr;
    int nCliLen = sizeof(tcp_cliaddr);
    getsockname(tcp_sockfd, &tcp_cliaddr, (socklen_t*)&nCliLen);
    inet_ntop(p->ai_family, get_in_addr(&tcp_cliaddr), s, sizeof s);
    int portnum = ntohs(get_in_addr_port(&tcp_cliaddr));
    
    printf("<%s> has TCP port %d and IP address %s for Phase I part 1\n", department_name.c_str(), portnum, s);
    printf("<%s> is now connected to the admission office\n", department_name.c_str());
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

//send to admission
void send_to_admission(string department_name){
    
    readFileAndSend(department_name);    //read file and send them
    
    //print message
    printf("Updating the admission office is done for <%s>\n", department_name.c_str());
    printf("End of Phase I for <%s>\n", department_name.c_str());
}

//UDP server
void udp_server(string department_name, int port){
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_sockfd < 0){
        perror("socket creation failed");
    }
    
    struct sockaddr_in udp_servaddr;
    memset(&udp_servaddr, 0, sizeof(udp_servaddr));
    
    udp_servaddr.sin_family = AF_INET;
    udp_servaddr.sin_addr.s_addr = INADDR_ANY;
    udp_servaddr.sin_port = htons(port);
    
    if(bind(udp_sockfd, (const struct sockaddr *)&udp_servaddr, sizeof(udp_servaddr)) < 0){
        perror("bind failed");
    }
    
    
    //<S#> has UDP port ... and IP address ... for Phase 2
    struct hostent *he;
    if ((he = gethostbyname("viterbi-scf2.usc.edu")) == NULL){
        perror("gethostbyname");
    }
    
    printf("<%s>  has UDP port %d and IP address %s for Phase2\n", department_name.c_str(), port, inet_ntoa(*(struct in_addr*)he->h_addr));
}

//receive message from admission
void receive_from_admission(string department_name){
    char buf[256] = {0};
    struct sockaddr_in udp_cliaddr;
    socklen_t len = sizeof(udp_cliaddr);
    //e.g. Student1#3.8#A1
    recvfrom(udp_sockfd, buf, 256, 0, (struct sockaddr *) &udp_cliaddr, &len);
    //<S#> has been admitted to <D#>
    //End of Phase 2 for <D#>
    printf("<Student%c> has been admitted to <%s>\n", buf[7], department_name.c_str());
    printf("End of Phase 2 for <%s>\n", department_name.c_str());
    close(udp_sockfd);
}

//main function
int main(int argc, char *argv[]){
    string strDepartments[] = {"departmentA", "departmentB", "departmentC"};
    int pid[3];
    for(int i = 0; i < 3; i++){
        pid[i] = fork();    //产生子进程
        if(pid[i] < 0){
            perror("ERROR on fork");
        }
        if(pid[i] == 0){
            connect_to_admission(strDepartments[i]);    //connect admission
            send_to_admission(strDepartments[i]);        //send data to admission
            close(tcp_sockfd);
            udp_server(strDepartments[i], g_department_port[i]);
            receive_from_admission(strDepartments[i]);
            exit(0);
        }
    }
    //wait all child socket
    int status;
    pid_t ret;
    for (int i = 0; i < 3; i++){
        do{
            usleep(100);
            ret = waitpid(pid[i], &status, WNOHANG);
        }while(ret == 0);
    }
    
    return 0;
}
