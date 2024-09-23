#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>



int main()
{
    struct addrinfo hints, *server;
    int r, sockfd;

    printf("Confugiring host");
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    /*connection with localhost*/
    r = getaddrinfo(0, "8080", &hints, &server);

    if(r!=0)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /*create socket*/
    printf("assign a socket");
    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    if(sockfd == -1)
    {
        perror("failed");
        exit(1);
    }

    puts("done");

    /*binding connection*/

    r = bind(sockfd, server->ai_addr, server->ai_addrlen);

    if (r==-1)
    {
        perror("failed");
        exit(1);
    }

    puts("done");


    /*listening for incoming connections*/

    r = listen(sockfd,1);

    if (r == -1)
    {
        perror("failed");
        exit(1);
    }

    puts("done");
    




}
