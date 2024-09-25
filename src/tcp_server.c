#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#define TRUE 1 
#define FALSE 0

int main()
{
    const char *port = "8080";
    
    const int clientname_size = 32;
    char clientname[clientname_size];
    char buffer[BUFSIZ], sendstr[BUFSIZ];
    const int backlog = 5; /*max connections*/
    char connection[backlog][clientname_size];
    socklen_t address_len = sizeof(struct sockaddr);
    struct addrinfo hints, *server;
    struct sockaddr address;
    int r, max_connect, fd,x, done;
    fd_set main_fd, read_fd;
    int serverfd, clientfd;

    /*configure the server*/
    printf("Configuring server...");
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    /*connection with localhost*/
    r = getaddrinfo(0, port, &hints, &server);

    if(r!=0)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /*create socket*/
    printf("assign a socket...");
    serverfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    if(serverfd == -1)
    {
        perror("failed");
        exit(1);
    }
    puts("done");
    /*binding connection*/
    printf("binding...");
    r = bind(serverfd, server->ai_addr, server->ai_addrlen);
    if (r==-1)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

    /*listening for incoming connections*/
     printf("Listening...");
    r = listen(serverfd,backlog);

    if (r == -1)
    {
        perror("failed");
        exit(1);
    }
    puts("done");

/* dealing with multiple connections*/
    max_connect = backlog;
    FD_ZERO(&main_fd); /*main file descriptor set*/
    FD_SET(serverfd, &main_fd); /*set servers file descriptor to main*/
    done = FALSE;
    while(!done)
    {
        read_fd = main_fd;

        r = select(max_connect + 1, &read_fd, NULL, NULL, 0);

        if (r == -1)
        {
            perror("failed");
            exit(1);
        }
        for(fd=1; fd <=max_connect; fd++)
        {
            if(FD_ISSET(fd,&read_fd))
            {
                if(fd==serverfd)
                {
                    clientfd = accept(serverfd, &address, &address_len);
                    if (clientfd==-1)
                    {
                        perror("failed");
                        exit(1);
                    }
                    r = getnameinfo(&address, address_len, clientname, clientname_size, 0, 0, NI_NUMERICHOST);
                    strcpy(connection[clientfd],clientname);
                    printf("New connection from %s\n", connection[clientfd]);

                    FD_SET(clientfd, &main_fd);

                    strcpy(buffer,"SERVER> Welcome ");
					strcat(buffer,connection[clientfd]);
					strcat(buffer," to the chat server\n");
					strcat(buffer,"SERVER> Type 'close' to disconnect; 'shutdown' to stop\n");
					send(clientfd,buffer,strlen(buffer),0);


                    strcpy(buffer,"SERVER> ");
					strcat(buffer,connection[clientfd]);
					strcat(buffer," has joined the server\n");

                    for( x=serverfd+1; x<max_connect; x++ )
					{
						if( FD_ISSET(x,&main_fd) )
							send(x,buffer,strlen(buffer),0);
					}
                    printf("%s", buffer);
                }
                else
                {
                    r = recv(fd, buffer, BUFSIZ, 0);

                    if(r<1)
                    {
                        FD_CLR(fd, &main_fd);
                        close(fd);

                        strcpy(buffer,"SERVER> ");
						strcat(buffer,connection[fd]);
						strcat(buffer," disconnected\n");
						/* loop through all connections (not the server) to send the string */
						for( x=serverfd+1; x<max_connect; x++ )
						{
							if( FD_ISSET(x,&main_fd) )
							{
								send(x,buffer,strlen(buffer),0);
							}
						}
						/* output the string locally */
						printf("%s",buffer);

                    }
                    else
                    {
                        buffer[r] = '\0';
                        if(strcmp(buffer, "shutdown\n")==0)
                        {
                            done = TRUE;
                        }
                        else
                        {
                            strcpy(sendstr,connection[fd]);
							strcat(sendstr,"> ");
							strcat(sendstr,buffer);
							/* loop through all connections, but not the server */
							for( x=serverfd+1; x<max_connect; x++ )
							{
								/* check for an active file descriptor */
								if( FD_ISSET(x,&main_fd) )
								{
									/* send the built string */
									send(x,sendstr,strlen(sendstr),0);
								}
							}
							/* echo the string to the server as well */
							printf("%s",sendstr);
                        }
                    }
                }
            }
        }
    }
   
    

   
    puts("TCP SERVER SHUTTING DOWN");

    
    /*close socket*/
    close(serverfd);
    /*free allocated memory*/
    freeaddrinfo(server);
    return(0);
}
