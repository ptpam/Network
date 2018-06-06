#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3490
#define BACKLOG 10

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in dest;
    int status,socket_fd, client_fd,num;
    socklen_t size;

    char buffer[10241];
    char buffer_send[10241];
    char *buff;
//  memset(buffer,0,sizeof(buffer));
    int yes =1;
    char * data = "data";
    char * connect = "connect";
    char * name = "name";
    char * close = "close";

    char client_name[10][20];
    // check {connect, name, data, close}
    int boolCheck[] = {0,0,0,0};
    int client_number = -1;
    int length;



    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
        fprintf(stderr, "Socket failure!!\n");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    memset(&server, 0, sizeof(server));
    memset(&dest,0,sizeof(dest));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY; 
    if ((bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr )))== -1)    { //sizeof(struct sockaddr) 
        fprintf(stderr, "Binding Failure\n");
        exit(1);
    }

    if ((listen(socket_fd, BACKLOG))== -1){
        fprintf(stderr, "Listening Failure\n");
        exit(1);
    }

    int boolArray[10];
    for(int i= 0; i < 10; i++){
        boolArray[i] = 0;
    }

    while(1) {

        size = sizeof(struct sockaddr_in);

        if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size))==-1 ) {
            perror("accept");
            exit(1);
        }
        printf("Server got connection from client %s\n", inet_ntoa(dest.sin_addr));

        while(1) {
                
                if ((num = recv(client_fd, buffer, 1024,0))== -1) {
                        perror("recv");
                        exit(1);
                }
                else if (num == 0) {
                        printf("Connection closed\n");
                        //So I can now wait for another client
                        break;
                }
                length = num;
                
                buffer[num] = '\0'; 

                char client_name_current[20];
                int data_current = -1;
                char str[20];
                char str2[20];
                if(buffer[1] == 'o'){   // connect
                    sscanf(buffer, "%s %d",str , &client_number);
                    sscanf(buffer, "%s %s",str , str2);
                    boolArray[client_number] = 1;
                    boolCheck[0] = 1;
                }else if(buffer[0] == 'n'){ // name
                    sscanf(buffer, "%s %d %s",str, &client_number, client_name_current);
                    strcpy(client_name[client_number], client_name_current);
                    boolCheck[1] = 1;
                }else if(buffer[0] == 'd'){ // data
                    sscanf(buffer, "%s %d %d",str, &client_number, &data_current);
                    sscanf(buffer, "%s %d %s",str, &client_number, str2);
                    boolCheck[2] = 1;
                }else if(buffer[0] == 'c'){ // close
                    sscanf(buffer, "%s %d",str, &client_number);
                    boolCheck[3] = 1;
                }

                if(boolCheck[0] == 0  && boolArray[client_number] != 1){
                    boolCheck[1] = 0;
                    boolCheck[2] = 0;
                    boolCheck[3] = 0;
                    strcpy(buffer_send, "there is no connection !!");
                } else if(boolCheck[0] == 1){   // connect
                    strcpy(buffer_send, "connection established ");
                    strcat(buffer_send, str2);
                    boolCheck[0] = 0;
                }else if(boolCheck[1] == 1){ // name
                    strcpy(buffer_send, "welcome ");
                    strcat(buffer_send, client_name[client_number]);
                    boolCheck[1] = 0;
                }else if(boolCheck[2] == 1){ // data
                    char c = client_number + '0';
                    strcpy(buffer_send, client_name[client_number]);
                    strcat(buffer_send, " ack ");
                    strcat(buffer_send, str2);
                    boolCheck[2] = 0;
                }else if(boolCheck[3] == 1){ // close
                    strcpy(buffer_send, "goodbye ");
                    strcat(buffer_send, client_name[client_number]);
                    boolCheck[3] = 0;
                    boolArray[client_number] = 0;
                }
                
                //strcpy(buffer_send, buffer);
                buffer_send[strlen(buffer_send)] = '\0'; 
                
                printf("Server:Msg Received %s\n", buffer);
                if ((send(client_fd,buffer_send, strlen(buffer_send),0))== -1) 
                {
                     fprintf(stderr, "Failure Sending Message\n");
                     //close(client_fd);
                     break;
                }

                printf("Server:Msg being sent: %s\nNumber of bytes sent: %d\n",buffer_send, strlen(buffer_send));

        } //End of Inner While...
        //Close Connection Socket
        //close(client_fd);
    } //Outer While

    //close(socket_fd);
    return 0;
}   