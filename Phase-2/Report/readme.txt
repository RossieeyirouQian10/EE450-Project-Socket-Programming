Name: Yirou Qian
USC ID: 4354716429
Project Phase 1

================================================================================

A. What I have done in the assignment.

In the Socket project, it will simulate a graduate student admission process in Admission, Department and Student. And in phase 1, Department should provide recruiting programs lists to the Admission through TCP sockets. Also, there are one admission and three departments; so I use fork() function to create child sockets and also store received files. Following the project requirements, I test the project on the server of "Viterbi-scf2.usc.edu".

================================================================================

B. What my code files are and what each one of them does. 

There are two cpp files: Admission.cpp and Department.cpp. 

In Admission.cpp: I use function fork() to create child sockets. First, I define the new socket. In advance, there should be static TCP port number of the admission. After get hostname and set bind information, bind and listen from department. Then clear the file information to prepare for receiving files from department. After receiving message from department, store them. And end the phase 1.

In Department.cpp: I also use function fork() to create three child sockets. First get socket address of IPv4, hardcode "Viterbi-scf2.usc.edu". Then read in .txt files and store information in another files. Each department should use dynamically assigned TCP port number, so three departments should get different TCP port number. Then send them to admission. Each time can only send one packet and update admission for department. After that, end child socket and end phase 1 for department.

================================================================================

C. What the TA should do to run my programs. (Any specific order of events should be mentioned.)

(a) All Compilation Steps on viterbi-scf2.usc.edu:
1. g++ -o admission Admission.cpp -lnsl -lresolv
2. g++ -o department Department.cpp -lnsl -lresolv

(b) Executing Steps on terminal:
1. Admission.cpp (server): ./admission
2. Department.cpp (client): ./department

================================================================================

D. The format of all the messages exchanged should follow the ones as given in the table.
Any idiosyncrasy of your project. It should say under what conditions the project fails, if any.

No.

================================================================================

E. Reused Code: Did I use code from anywhere for my project? 

Yes.

The most part of code in Admission.cpp and Department.cpp are based on the code of Beej's Guide to C Programming and Network Programming (socket programming), which are in the 6th chapter, especially from "6.1. A Simple Stream Server" and "6.2. A simple Stream Client". Also, there are some code from Lab1 and Lab2.

In Admission.cpp:
From line 52 to line 73 and from line 99 to line 128: It is based on Beej's Guide to C Programming and Network Programming (socket programming):"6.1. A Simple Stream Server" and Lab2. I also do some modification on the original code.
From line 65 to line 78: It is based Lab1.

In Department.cpp: 
From line 46 to line 59 and from line 85 to line 131: It is based on Beej's Guide to C Programming and Network Programming (socket programming):"6.2. A simple Stream Client" and Lab2. I also do some modification on the original code.
From line 65 to line 78: It is based Lab1.



 