/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid, hit, index,i,ii,j;
     socklen_t clilen;
     char buffer[4096],buffer1[10][4096*3],safebuffer[4096*4];
     struct sockaddr_in serv_addr, cli_addr;
     struct hostent *he;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");


     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);

     for(hit=1; ;hit++){
        
        clilen = sizeof(cli_addr);
        if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))<0)
            error("ERROR on accept");
        printf("client address is:%s\n",inet_ntoa(cli_addr.sin_addr));
        he = gethostbyaddr(&cli_addr.sin_addr, sizeof(cli_addr.sin_addr), AF_INET);
        printf("Host name: %s\n", he->h_name);




        //find the client's address hostname
        bzero(buffer1[index],4095);
        sprintf(buffer1[index],"<p>Client address: %s</p>",inet_ntoa(cli_addr.sin_addr));
        strcat(buffer1[index],"<p>Host name:");
        strcat(buffer1[index], he->h_name);
        strcat(buffer1[index],"</p>");





        bzero(buffer,4096);
        n = read(newsockfd,buffer,4096);
        if (n < 0) error("ERROR reading from socket");
        char * pch;

        //filter the input buffer to avoid Injection Attacks
        j= strlen(buffer);
        ii=0;
        for(i=0; i<j;i++){
            if(buffer[i]=='<'){
                safebuffer[ii++]='&';
                safebuffer[ii++]='l';
                safebuffer[ii++]='t';
            }else if(buffer[i]=='>'){
                safebuffer[ii++]='&';
                safebuffer[ii++]='g';
                safebuffer[ii++]='t';
            }else if(buffer[i]=='"'){
                safebuffer[ii++]='&';
                safebuffer[ii++]='q';
                safebuffer[ii++]='u';
                safebuffer[ii++]='o';
                safebuffer[ii++]='t';
            }else{
                safebuffer[ii++]=buffer[i];
            }

        }

        
        //Find the User-Agent information
        pch = strstr (safebuffer,"User-Agent:");
        j= strlen(pch);
        for(i=0; i<j; i++){
            if(pch[i]=='\n'){
                break;
            }
        }
        char buffersend[i+1];
        for(j=0;j<=i;j++){
            buffersend[j]=pch[j];
        }
        buffersend[j]='\0';
        strcat(buffer1[index],"<p>");
        strcat(buffer1[index],buffersend);
        strcat(buffer1[index],"</p>");
        index = (index+1)%10;




        if((pid=fork())<0){
            error("ERROR on fork");
        }
        else{
            if(pid == 0){//child
                close(sockfd);

                printf("Here is the  message: %s\n",buffer);



                
                if( !strncmp(buffer,"GET / HTTP/1.1",14)  ){
                    bzero(buffer,4096);
                    sprintf(buffer,"HTTP/1.1 200 OK\n Content-type: text/html\n\n\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    bzero(buffer,4096);
                    sprintf(buffer, "<html><body><h1>List of web (Total %d hits from browser)</h1></body>\n",hit);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    for(int i=0;i<10;i++){
                        if(buffer1[i][0]==0) break;
                        bzero(buffer,4096);
                        sprintf(buffer, "<p>%d record<p>\n",i+1);
                        send(newsockfd, buffer, strlen(buffer), 0);
                        send(newsockfd, buffer1[i], strlen(buffer1[i]), 0);
                        sprintf(buffer, "<br>");
                        send(newsockfd, buffer, strlen(buffer), 0);

                    }
                    sprintf(buffer, "<p>end<p><br><br><A href = '/about.html'>About me</A></body></html>\n");
                    send(newsockfd, buffer, strlen(buffer), 0);

                } else if(!strncmp(buffer,"GET /about.html",15) ){
                    bzero(buffer,4096);
                    sprintf(buffer,"HTTP/1.1 200 OK\n Content-type: text/html\n\n\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    bzero(buffer,4096);
                    sprintf(buffer, "<html><body><h1>About me</h1><p>&nbsp&nbspMy name is Zhihui Qiu. </p>\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    bzero(buffer,4096);
                    sprintf(buffer, "<p>&nbsp&nbsp I am a graduate student from EE department.</p>\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    bzero(buffer,4096);
                    sprintf(buffer, "<br><br><A href = '/'>List of Web</A></body></html>\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                }






            }else{//parent
                close(newsockfd);
            }
        }

     }
     
        
}