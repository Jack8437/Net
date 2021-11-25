/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
/*
 * Chat.c - Jack Schwartz (Jaeschwa)
 * CREATED: 9/23/2021
 *
 * This program is for the chat function of Netster
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define BUFFER_SIZE 256
void chat_server(char* iface, long port, int use_udp) {
  bool done = false;
  char *ip;
  int counter = 0;
  int socket_desc , client_sock , c;
  struct sockaddr_in server , client;
  char client_message[BUFFER_SIZE], message[BUFFER_SIZE];
  // create socket
  if(use_udp==0) {
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  //printf("%s\n", inet_ntoa(server.sin_addr));
  bind(socket_desc,(struct sockaddr *)&server , sizeof(server));
  if(use_udp==0) {
    while(1) {
      done=false;
      listen(socket_desc , 3);
      c = sizeof(struct sockaddr_in);
      client_sock = accept(socket_desc, (struct sockaddr *)&server, (socklen_t*)&c);
      ip = inet_ntoa(server.sin_addr);
      printf("connection %d from (%s,%ld)\n", counter,ip, port); 
      counter++;
      while(!done) {
        memset(client_message,0,BUFFER_SIZE);
        memset(message,0,BUFFER_SIZE);
        if(read(client_sock , client_message , BUFFER_SIZE)<0) {
          printf("Read failed\n");
	  return;
	}
        //printf("Client message: %s\n", client_message);
        printf("got message from (%s,%ld)\n", ip, port);
        if(strcmp(client_message,"hello")==0) {
          //printf("here\n");
          strcpy(message,"world");
          send(client_sock,message,BUFFER_SIZE,0);
        } else if(strcmp(client_message,"goodbye")==0) {
          strcpy(message,"farewell");
          send(client_sock,message,BUFFER_SIZE,0);
	  close(client_sock);
          done=true;	
        }else if(strcmp(client_message,"exit")==0) {
          strcpy(message,"ok");
	  send(client_sock,message,BUFFER_SIZE,0);
	  close(socket_desc);
	  close(client_sock);
	  return;
        }else {
          strcpy(message,client_message);
          send(client_sock , message , BUFFER_SIZE, 0);
        }
      }
    }
  } else {
    int len;
    len = sizeof(client);
    while(1) {
      memset(client_message, 0, BUFFER_SIZE);
      memset(message, 0, BUFFER_SIZE);
      recvfrom(socket_desc, client_message, BUFFER_SIZE, 0, (struct sockaddr*) &server, (socklen_t *) &len);
      printf("got message from (%s,%ld)\n", inet_ntoa(server.sin_addr), port);
      if(strcmp(client_message,"hello")==0) {
        strcpy(message,"world");
	sendto(socket_desc, message, BUFFER_SIZE, 0, (struct sockaddr *) &server, len);
      } else if(strcmp(client_message,"goodbye")==0) {
        strcpy(message,"farewell");
	sendto(socket_desc, message, BUFFER_SIZE, 0, (struct sockaddr *) &server, len);
      } else if(strcmp(client_message, "exit")==0) {
	strcpy(message,"ok");
	sendto(socket_desc, message, BUFFER_SIZE, 0, (struct sockaddr *) &server, len);
        close(socket_desc);
	return;
      }else {
        strcpy(message,client_message);
	sendto(socket_desc, message, BUFFER_SIZE, 0, (struct sockaddr *) &server, len);
      }
      //printf("message sent\n");
    }
    return;
  }
}
void chat_client(char* host, long port, int use_udp) {
  int sock;
  struct sockaddr_in server;
  char message[BUFFER_SIZE] , server_reply[BUFFER_SIZE];
  // create socket
  if(use_udp==0) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset((char *) &server, 0, sizeof(server));
  }
  inet_pton(AF_INET, host, &server.sin_addr);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  //printf("%s\n", inet_ntoa(server.sin_addr));
  if(use_udp==0) {
    connect(sock , (struct sockaddr *)&server , sizeof(server));
    while(1){
      memset(message,0,BUFFER_SIZE);
      memset(server_reply,0,BUFFER_SIZE);
      if(scanf("%s" , message)==-1) {
        printf("Scanf failed\n");
	return;
      }
      if( send(sock , message , strlen(message) , 0) < 0) {
        printf("Send failed\n");
        return;
      }
      if( read(sock , server_reply , BUFFER_SIZE) < 0) {
        printf("recv failed\n");
        break;
      }
      //printf("Server: %s, message: %s\n", server_reply, message);
      printf("%s\n", server_reply);
      if(strcmp(message,"goodbye")==0 || strcmp(message,"exit")==0) {
        return;
      }
    }
  } else {
    int len;
    len = sizeof(server);
    while(1) {
      memset(message,0,BUFFER_SIZE);
      memset(server_reply,0,BUFFER_SIZE);
      if(scanf("%s",message)==-1) {
        printf("Scanf failed\n");
	return;
      }
      sendto(sock, message, BUFFER_SIZE, 0, (const struct sockaddr *) &server, len);
      recvfrom(sock, server_reply, BUFFER_SIZE, 0, (struct sockaddr *) &server, (socklen_t *)&len);
      printf("%s\n", server_reply);
      if(strcmp(message,"goodbye")==0 || strcmp(message,"exit")==0) {
        close(sock);
	return;
      }
    }
    return;
  }
}
