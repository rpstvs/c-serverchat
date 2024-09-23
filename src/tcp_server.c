#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>



int main()
{
    struct addrinfo hints, *server;
    int r, sockfd, clientfd;
    struct sockaddr client_address;
    socklen_t client_len;
    const int buffer_size= 1024;
    char buffer[buffer_size];
    const char *http_data =
    	"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/html\r\n\r\n"
		"<h1>Hello from your server!</h1>";


    printf("Configuring host...");
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
    printf("assign a socket...");
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

    /*free allocated memory*/
    freeaddrinfo(server);
    /*close socket*/
    close(sockfd);
    puts("Socket closed, done");
    return(0);
}
