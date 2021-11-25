#include "file.h"
#include <stdio.h>
/*
 *  Here is the starting point for your netster part.2 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
// Read and send the file on the client side
// recieve and write the file on the server side


/*
 * File.c - Jack Schwartz jaeschwa
 * CREATED: 8/28/2021
 * 
 * This file is used to send files between two computers.
 * Only the client can send a file and only the server can recieve a file
 * Client also prints the file they are sending
 */
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER 256
void file_server(char* iface, long port, int use_udp, FILE* fp) {
  int e;
  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr, client_addr;
  socklen_t addr_size;
  // Set socket based on if UDP is being used or TCP
  if(use_udp==0) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  }
  if(sockfd<0) {
    printf("Error in socket\n");
    exit(1);
  }
  // Set up the port and IP
  server_addr.sin_family = AF_INET;
  //server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //server_addr.sin_addr.s_addr = inet_ntoa(iface);
  server_addr.sin_port = htons(port);
  e = bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if(e<0) {
    printf("Error in bind\n");
    exit(1);
  }
  // Split based on using UDP on TCP
  // Both are the same with how they write to the file
  // Only difference is how they recieve the file
  if(use_udp==0) {
    if(listen(sockfd, 10)!=0) {
      printf("Error in listen\n");
      exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    int n;
    void *buffer = (void *) malloc(BUFFER);
    while(1) {
      n = recv(new_sock, buffer, BUFFER, 0);
      if(n<=0) {
        break;
	return;
      }
      fwrite(buffer, 1, n, fp);
      bzero(buffer, BUFFER);
    }
    close(new_sock);
    close(sockfd);
    return;
  } else {
    //printf("Here\n");
    int n;
    void *buffer = (void *) malloc(BUFFER);
    while(1) {
      addr_size = sizeof(client_addr);
      n = recvfrom(sockfd, buffer, BUFFER, 0, (struct sockaddr*) &client_addr, &addr_size);
      if(n<=0) {
        break;
	return;
      }
      //printf("%d\n", n);
      fwrite(buffer, 1, n, fp);
      bzero(buffer, BUFFER);
    }
    close(sockfd);
    return;
  }
}

void file_client(char* host, long port, int use_udp, FILE* fp) {
  int e;
  int sockfd;
  struct sockaddr_in server_addr;
  // Set up the socket based on UDP or TCP
  if(use_udp==0) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  }
  if(sockfd<0) {
    printf("Error in socket\n");
    exit(1);
  }
  // Set up the IP and the port
  //inet_pton(AF_INET, host, &server_addr.sin_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // Both are mostly the same
  // main different comes with how they send the file
  // Both read the file and store the contents of the file the same way
  if(use_udp==0) {
    e = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(e==-1) {
      exit(1);
    }
    void *data = malloc(BUFFER);
    int c;
    int d;
    do {
      c = fread(data, 1, BUFFER, fp);
      d = send(sockfd,data,c,0);
      if(d==-1){
        printf("Error on send\n");
	exit(1);
      }
      printf("%s", (char *) data);
      bzero(data,BUFFER);
    } while(c==BUFFER);
    close(sockfd);
    return;
  } else {
    void *data = malloc(BUFFER);
    int c;
    int d;
    do {
      c = fread(data, 1, BUFFER, fp);
      //printf("%d\n", c);
      d = sendto(sockfd, data, c, 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
      if(d==-1) {
        printf("Error on send\n");
	exit(1);
      }
      printf("%s", (char *) data);
      bzero(data, BUFFER);
    } while(c==BUFFER);
    //strcpy(data, "TEST");
    sendto(sockfd,data,0,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
    close(sockfd); 
    return;
  }
}
