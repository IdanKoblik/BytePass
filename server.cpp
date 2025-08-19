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
#include "utils.h"

#define BUFFER 4096

int runServer(const unsigned int port, const std::string &outputDir) {
   GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (port < MIN_PORT || port > MAX_PORT) {
      std::cerr << "Invalid port!" << std::endl;
      return -1;
    }

   int sockfd;
   struct sockaddr_in serverAddr, clientAddr;

   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      std::cerr << "socket creation failed" << std::endl;
      return -1;
   }

   memset(&serverAddr, 0, sizeof(serverAddr));
   memset(&clientAddr, 0, sizeof(clientAddr));

   serverAddr.sin_family = AF_INET; // IPv4 
   serverAddr.sin_addr.s_addr = INADDR_ANY; 
   serverAddr.sin_port = htons(port); 

   if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
      std::cerr << "Bind failed" << std::endl;
      return -1;
   }

   sockaddr_in boundAddr{};
   socklen_t len = sizeof(boundAddr);
   if (getsockname(sockfd, reinterpret_cast<sockaddr*>(&boundAddr), &len) == 0) {
    std::cout << "Server running on "
              << inet_ntoa(boundAddr.sin_addr)
              << ":" << ntohs(boundAddr.sin_port)
              << std::endl;
   } else {
      std::cerr << "getsockname failed" << std::endl;
   }

   std::map<int, std::string> chunks;
   std::string filename;

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
         std::cerr << "recvfrom failed" << std::endl;
         continue;
      }

      if (n > BUFFER) {
         std::cerr << "Received packet larger than buffer — dropping\n";
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
   
      if (filename.empty()) 
         filename = outputDir + "/" + chunk.filename();

      chunks[chunk.index()] = chunk.data();
      if (chunk.index() == -1) {
         std::cout << "Final chunk marker received. Writing file..." << std::endl;

         std::ofstream ofs(filename, std::ios::binary | std::ios::out | std::ios::trunc);
         if (!ofs) {
            std::cerr << "Failed to open output file " << filename << std::endl;
            break;
         }

         for (int i = 0; i < chunks.size() - 1; i++) {
            std::string &chunk = chunks[i];
            ofs.write(chunk.data(), chunk.size());
         }

         std::string &finalChunk = chunks[-1];
         ofs.write(finalChunk.data(), finalChunk.size());
          
         ofs.close();
         std::cout << "File reconstructed: " << filename << std::endl;

         std::cout << "Checking file checksum..." << std::endl;
         const std::string checksum = calcChecksum(filename);
         std::cout << "Received file - " << checksum << std::endl;

         if (checksum == chunk.checksum())
            std::cout << "Checksum is valid!" << std::endl;
         else
            std::cerr << "Checksum is invalid!" << std::endl;

         break;
      }
     
   }

   google::protobuf::ShutdownProtobufLibrary();
   close(sockfd);
   return 0;
}
