#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>

#define BUFFERLENGTH 10000
#define SERVERPORTJAVA 55555
#define SERVERPORTCPP 55556
#define FILENAME "Data.txt"

using namespace std;

//1->CPP, 2->JAVA
int serverChoice;

void* runServer(void* arg)
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char receiveBuffer[BUFFERLENGTH];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(receiveBuffer, '0', sizeof(receiveBuffer)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVERPORTCPP); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 1); 

    while(1)
    {
        cout << "Server waiting for connections..." << endl;

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        //Get file name
        int readCharacters = read(connfd, receiveBuffer, sizeof(receiveBuffer) - 1);

        string fileName(receiveBuffer, readCharacters);

        //Open file for writing with the specified file name
        ofstream fileWriter;

        char sep = '\n';

        for(size_t p=0, q=0; p!=fileName.npos; p=q)
        {
            string temp = fileName.substr(p+(p!=0), (q=fileName.find(sep, p+1))-p-(p!=0));

            if(p == 0)
            {
                int extIndex = temp.find_last_of(".");
                string ext = temp.substr(extIndex);
                temp = temp.substr(0, extIndex);
                temp += " - received" + ext;

                fileWriter.open(temp.c_str(), ios::trunc);
            }
            else
            {
                fileWriter << temp;
            }
        }

        cout << "GOing in while"<<endl;

        //Write into file uptill no data left
        while((readCharacters = read(connfd, receiveBuffer, sizeof(receiveBuffer) - 1)) > 0)
        {
            string temp(receiveBuffer, readCharacters);
            fileWriter << temp;
        }

        fileWriter.close();
        close(connfd);

        cout << "File successfully received" << endl;
    }
}

void* runClient(void* arg)
{
    int sockfd = 0;
    char sendBuffer[BUFFERLENGTH + 1];
    struct sockaddr_in serv_addr; 

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error : Unable to create socket\n");
        return NULL;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;

    //Select the port on the basis of server specified by the user
    if(serverChoice == 1)
    {
        serv_addr.sin_port = htons(SERVERPORTCPP); 
    }
    else if(serverChoice == 2)
    {
        serv_addr.sin_port = htons(SERVERPORTJAVA);
    }
    else
    {
        cout << "Error: Invalid server selected" << endl;
        return NULL;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("Error: Unable to set the client socket address\n");
        return NULL;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("Error : Connection with the server Failed\n");
       return NULL;
    } 

    //Open the file to be sent to the server
    ifstream fileReader;
    fileReader.open(FILENAME);

    //Send the filename to the server
    write(sockfd, FILENAME, strlen(FILENAME));
    write(sockfd, "\n", 1);

    //Read the file line by line and send it to the server
    string line;
    while ( getline (fileReader, line) )
    {
        memset(sendBuffer, '0',sizeof(sendBuffer));
        strcpy(sendBuffer, line.c_str());
        write(sockfd, sendBuffer, strlen(sendBuffer)); 
    }

    fileReader.close();
    close(sockfd);

    cout << "File sent by the client" << endl;
}

int main()
{
    pthread_t serverThread, clientThread;

    //Ask the user to select the server on which he would like to upload the file
    cout << "Select the server to send the file:\n1. CPP\n2. JAVA" << endl;
    int choice;
    scanf("%d", &choice);

    serverChoice = choice;
    
    int returnVal = pthread_create( &serverThread, NULL, runServer, NULL);
    if(returnVal)
    {
        cout << "An error occured" << endl;
        exit(-1);
    }

    returnVal = pthread_create( &clientThread, NULL, runClient, NULL);
    if(returnVal)
    {
        cout << "An error occured" << endl;
        exit(-1);
    }

    //Wait for the threads to finish
    pthread_join(serverThread, NULL);
    pthread_join(clientThread, NULL);
}