
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define PORT 8080
#define BUFFER_SIZE 256

void cipher(char *message) {
    int shift = 3;
    while (*message) {
        if (*message >= 'A' && *message <= 'Z') {
            *message = (*message - 'A' + shift) % 26 + 'A';
        }
        else if (*message >= 'a' && *message <= 'z') {
            *message = (*message - 'a' + shift) % 26 + 'a';
        }
        message++;
    }
}

int main(int argc,char*argv[]) {
    int server_fd, client_socket, valread;
    struct sockaddr_in server_addr,client_addr;
    int server_len = sizeof(server_addr);
    int client_len=sizeof(client_addr);
    char input_buf[BUFFER_SIZE] = {0};
    char hello[] = {"Hello from server.\n"};
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    if ((client_socket = accept(server_fd, NULL, NULL)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
    recv(client_socket, input_buf, BUFFER_SIZE, 0);
    printf("From Client: %s\n", input_buf);
    send(client_socket,hello,sizeof(hello),0);
    while(1){
        valread = recv(client_socket, input_buf, BUFFER_SIZE, 0);
        if(!strcmp(input_buf,"exit")){
            strcpy(input_buf,"byebye.\n");
            send(client_socket, input_buf, strlen(input_buf), 0);
            getpeername(client_socket,(struct sockaddr*)&client_addr,&client_len);
            printf("Client %s:%d  Disconnected\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        }
        else if (valread != 0) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                close(pipefd[0]); // Close the read end of the pipe
                if(strlen(input_buf)==0){
                    printf("No command received.\n");
                }
                cipher(input_buf);
                write(pipefd[1], input_buf, strlen(input_buf) + 1);
                exit(0);
            }
            else {
            // Parent process
            close(pipefd[1]); // Close the write end of the pipe
            read(pipefd[0], input_buf, BUFFER_SIZE);
            int status;
            wait(&status); 
            printf("Get encoded message from child process: %s\n", input_buf);
            send(client_socket, input_buf, strlen(input_buf), 0);
            printf("Encoded message sent.\n");
            }
        }
        
    }
    close(server_fd);
    return 0;
}

