#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "server.h"

#define BUFFER 1024

int runServer(void) {
   int sockfd;
   struct sockaddr_in serverAddr, clientAddr;

   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket creation failed"); 
      exit(EXIT_FAILURE);    
      return -1;
   }

   memset(&serverAddr, 0, sizeof(serverAddr));
   memset(&clientAddr, 0, sizeof(clientAddr));

   serverAddr.sin_family = AF_INET; // IPv4 
   serverAddr.sin_addr.s_addr = INADDR_ANY; 
   serverAddr.sin_port = htons(PORT); 

   if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
      perror("bind failed"); 
      exit(EXIT_FAILURE); 
      return -1;
   }

   while (true) {
      char buffer[BUFFER]; 
      socklen_t len = sizeof(clientAddr);
      int n = recvfrom(
          sockfd,
          (char *)buffer - 1, 
          BUFFER, 
          MSG_WAITALL, 
          (struct sockaddr *)&clientAddr, 
          &len
      );

      if (n < 0) {
         perror("recvfrom failed");
         continue;
      }

      buffer[n] = '\0';

      printf("Received message from %s:%d\n",
        inet_ntoa(clientAddr.sin_addr),
        ntohs(clientAddr.sin_port)
      );

      std::cout << buffer << std::endl; 
   }

   return 0;
}
