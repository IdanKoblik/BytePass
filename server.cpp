#include <absl/strings/internal/str_format/extension.h>
#include <bits/stdc++.h> 
#include <google/protobuf/message_lite.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "server.h"
#include "filechunk.pb.h"

#define BUFFER 4096

int runServer(void) {
   GOOGLE_PROTOBUF_VERIFY_VERSION;

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
          buffer, 
          BUFFER, 
          MSG_WAITALL, 
          (struct sockaddr *)&clientAddr, 
          &len
      );

      if (n < 0) {
         perror("recvfrom failed");
         continue;
      }

      protocol::FileChunk chunk;
      if (!chunk.ParseFromArray(buffer, n)) {
         std::cerr << "Failed to parse protobuf message" << std::endl;
         continue;
      }

      std::string clientIP = inet_ntoa(clientAddr.sin_addr);
      int clientPort = ntohs(clientAddr.sin_port);

      std::cout << "Received chunk from " << clientIP << ":" << clientPort << std::endl;
      std::cout << "CHUNK: " << chunk.index() << std::endl;
      std::cout << "Filename: " << chunk.filename() << std::endl;
      std::cout << "Data size: " << chunk.data().size() << " bytes" << std::endl;
   }

   google::protobuf::ShutdownProtobufLibrary();
   close(sockfd);
   return 0;
}
