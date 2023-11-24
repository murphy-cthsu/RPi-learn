
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main(int argc,char*argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==-1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    strcpy(buffer, "Hello, server!");
    if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer));
    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    printf("Server response: %s\n", buffer);
    while(1){
        char recv_mes[256]={};
        // recv(sockfd,&recv_mes,sizeof(recv_mes),0);
        // printf("Server response: %s\n", recv_mes);
        char command[256]={};
        strcpy(command,argv[1]);//Assume the first argument as the command
        printf("Enter command:%s\n",command);
        send(sockfd,&command,sizeof(command),0);
        recv(sockfd,&recv_mes,sizeof(recv_mes),0);
        printf("Encoded Message: %s\n", recv_mes);
        strcpy(command,"exit");
        send(sockfd,&command,sizeof(recv_mes),0);
        recv(sockfd,&recv_mes,sizeof(recv_mes),0);
        printf("Server response: %s\n", recv_mes);
        break;
    }
    // Close the socket
    close(sockfd);

    return 0;
}
