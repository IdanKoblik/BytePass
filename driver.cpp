#include <bits/stdc++.h> 
#include <cstdint>
#include <fstream>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "driver.h"
#include "filechunk.pb.h"
#include "net.h"

#define BUFFER_SIZE 1024

int runDriver(const std::string &addr, const std::string &filename) {
   GOOGLE_PROTOBUF_VERIFY_VERSION;

   std::ifstream file(filename, std::ios::binary | std::ios::ate);
   if (!file) {
      std::cerr << "Cannot open selected file" << std::endl;
      return -1;
   }

   int sockfd;
   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      std::cerr << "Socket creation failed" << std::endl;
      return -1;
   }
  
   const sockaddr_in serverAddr = parseAddress(addr);
   std::streamsize fileSize = file.tellg();
   file.seekg(0, std::ios::beg);

   std::size_t chunksCount = (fileSize + BUFFER_SIZE - 1) / BUFFER_SIZE;
   for (std::size_t i = 0; i < chunksCount; i++) {
      std::array<uint8_t, BUFFER_SIZE> buffer{};
      std::streamsize toRead = std::min<std::streamsize>(BUFFER_SIZE, fileSize - i * BUFFER_SIZE);
      file.read(reinterpret_cast<char *>(buffer.data()), toRead);

      std::cout << "Chunk " << i << ": read " << toRead << " bytes" << std::endl;

      protocol::FileChunk chunk;
      chunk.set_filename(filename);
      chunk.set_data(buffer.data(), toRead);
      chunk.set_index(i);

      std::string serializedChunk;
      if (!chunk.SerializeToString(&serializedChunk)) {
         std::cerr << "Failed to serialize chunk " << i << std::endl;
         continue;
      }

      sendto(
         sockfd,
         serializedChunk.data(), 
         serializedChunk.size(), 
         MSG_CONFIRM, 
         (const struct sockaddr *)&serverAddr, 
         sizeof(serverAddr)
      );
   }

   file.close();
   close(sockfd);
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}
