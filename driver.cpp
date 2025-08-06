#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "driver.h"

int runDriver(void) {
   int sockfd;
   struct sockaddr_in serverAddr;

   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket creation failed"); 
      exit(EXIT_FAILURE); 
      return -1;
   }
   
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET; 
   serverAddr.sin_port = htons(PORT); 
   serverAddr.sin_addr.s_addr = INADDR_ANY; 

   const char *hello = "hey";
   for (int i = 0; i < 1000; i++) {
      sendto(
          sockfd,
          hello, 
          strlen(hello), 
          MSG_CONFIRM, 
          (const struct sockaddr *) &serverAddr, 
          sizeof(serverAddr)
      );
   }

   close(sockfd);
   return 0;
}
