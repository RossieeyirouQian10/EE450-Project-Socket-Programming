Name: Yirou Qian
USC ID: 4354716429
Project Phase 2

================================================================================

A. What I have done in the assignment.

In the Socket project, it will simulate a graduate student admission process in Admission, Department and Student. And in phase 2, after admission gives departments the information about programs, student will send their applications to admission. And admission will make the decisions about these applications following some standards, then admission will tell the results both to students and departments with both TCP and UDP sockets. Following the project requirements, I test the project on the server of "Viterbi-scf2.usc.edu".

================================================================================

B. What my code files are and what each one of them does. 

There are two cpp files: Admission.cpp and Department.cpp. 

In Admission.cpp: I use function fork() to create child sockets. First, get socket address and define some helper functions to be used in the following TCP and UDP sockets code. Then define the TCP server which is the same as that in project phase 1. Then construct UDP socket and clear file to prepare to store information from department because admission must first connect to department and then communication with students. After that, admission read information from department and use child socket to deal with there different departments' information. Besides, admission also need to receive message from students with child socket to handle five different students. after that part, admission can decide to accept or refuse the application from students and also tell the results to departments with dynamic UDP ports.

In Department.cpp: I also use function fork() to create three child sockets. First get socket address of IPv4, hardcode "Viterbi-scf2.usc.edu". Then read in .txt files and store information in another files. Each department should use dynamically assigned TCP port number, so three departments should get different TCP port number. Then send them to admission. Each time can only send one packet and update admission for department. After that, create UDP server to receive information from admission. These message are about whether students' applications are accepted or refused with static UDP ports.

In Student.cpp: in this part, function fork() is still needed. Get the socket address, and connect to the admission, which is similar to Department.cpp. Student.cpp also need to read in students files and store some information, also send these information to admission. Student can communication with admission with dynamic TCP ports and then get the applications' results from admission with static UDP ports.

================================================================================

C. What the TA should do to run my programs. 

(a) All Compilation Steps on viterbi-scf2.usc.edu:
1. g++ -o admission Admission.cpp -lnsl -lresolv
2. g++ -o department Department.cpp -lnsl -lresolv
3. g++ -o student Student.cpp -lnsl -lresolv

(b) Executing Steps on terminal:
1. Admission.cpp: ./admission
2. Department.cpp: ./department
3. Student.cpp: ./student

================================================================================

D. The format of all the messages exchanged should follow the ones as given in the table.
Any idiosyncrasy of your project. It should say under what conditions the project fails, if any.

No.

================================================================================

E. Reused Code: Did I use code from anywhere for my project? 

Yes.

The most part of code in Admission.cpp, Department.cpp and Student.cpp are based on the code of Beej's Guide to C Programming and Network Programming (socket programming), which are in the 6th chapter, especially from "6.1. A Simple Stream Server" and "6.2. A simple Stream Client". Also, there are some code from Lab1, Lab2 and project phase 1.

In Admission.cpp:
From line 57 to line 71 and from line 111 to line 135 and from line 203 to line 242: It is based on Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A Simple Stream Server" and Lab2,as well as project phase 1. I also do some modification on the original code.
From line 159 to line 175: It is based on project phase 1 and Lab1.
From line 88 to line 102: It is based on Lab4.

In Department.cpp: 
From line 54 to line 67 and from line 95 to line 140: It is based on Beej's Guide to C Programming and Network Programming (socket programming):"6.2. A simple Stream Client" and Lab2. I also do some modification on the original code.
From line 73 to line 87: It is based on project phase 1 and Lab1.

In Student.cpp:
From line 53 to line 119: It is based on Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A Simple Stream Server" and Lab2,as well as project phase 1. I also do some modification on the original code.
From line 121 to line 132: It is based on project phase 1 and Lab1.


 