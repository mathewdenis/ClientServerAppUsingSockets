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
#define SERVERPORT 55556
#define FILENAME "Data.txt"

using namespace std;

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
    serv_addr.sin_port = htons(SERVERPORT); 

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
        printf("\n Error : Could not create socket \n");
        return NULL;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVERPORT); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return NULL;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
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
        //write(sockfd, "\n", 1);
    }

    fileReader.close();
    close(sockfd);

    cout << "File sent by the client" << endl;
}

int main()
{
    pthread_t serverThread, clientThread;
    
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