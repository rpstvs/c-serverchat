#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>

#define TRUE 1
#define FALSE 0

int main()
{
    

    int r,sockfd,x, done;
    struct addrinfo hints, *host;
    fd_set read_fd;
    char buffer[BUFSIZ];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    r = getaddrinfo(0, "8080", &hints, &host);

    if(r!=0)
    {
        perror("TCP Client");
        exit(1);
    }

    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);

    if (sockfd == -1)
    {
        perror("TCP Client");
        exit(1);
    }

    r = connect(sockfd, host->ai_addr, host->ai_addrlen);

    if(r == -1)
    {
        perror("TCP Client");
        exit(1);
    }
    done = FALSE;
    while(!done)
    {
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd);
        FD_SET(0, &read_fd);
        r = select(sockfd + 1, &read_fd, NULL, NULL, 0);

        if (r==-1)
        {
            perror("failed");
            exit(1);
        }
        /*remote input*/

        if(FD_ISSET(sockfd, &read_fd))
        {
            r = recv(sockfd, buffer, BUFSIZ, 0);
            if (r < 1)
            {
                puts("connection closed by peer");
                break;
            }
            buffer[r] = '\0';
			printf("%s",buffer);
        }
		/* local input  */
		
        if(FD_ISSET(0, &read_fd))
        {
            if(fgets(buffer, BUFSIZ, stdin) == NULL)
            {
                putchar('\n');
            }
            else if(strcmp(buffer, "close\n")==0)
            {
                done=TRUE;
            }
            else
            {
                send(sockfd, buffer,strlen(buffer),0);
            }
        }
        
    }

   
    close(sockfd);

    return(0);
}