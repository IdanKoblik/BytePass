#include <iostream>
#include <ostream>
#include <stdexcept>
#include "flags.h"
#include "mode.h"
#include "prompt.h"
#include "driver.h"
#include "server.h"

std::string sanitizeFilename(const std::string &input);
std::string trim(const std::string& str);

int main(int argc, char *argv[]) {
   if (argc < 1) 
      return -1;

   if (argc == 1) {
      int choice;
      std::cout << "=== Select option ===" << std::endl;
      std::cout << "1. Sender" << std::endl;
      std::cout << "2. Receiver" << std::endl;
      std::cout << ">>> ";
      std::cin >> choice;

      switch (choice) {
         case 1: {
            std::vector<std::string> initial = {"Addr", "Filename"};
            PromptOptions options(initial);
            options.promptOptions(argc, argv);
            runDriver(trim(options.getCtx("Addr")), trim(sanitizeFilename(options.getCtx("Filename"))));
            break;
         }
         case 2: {
            int port;
            std::cout << "Port: ";
            std::cin >> port;

            runServer(port);
            break;
         }
         default:
            std::cerr << "Invalid mode" << std::endl; 
            return -1;
      }

      return 0;
   }

   std::vector<std::string> initial = {"mode", "filename", "addr"};
   FlagOptions options(initial);
   options.promptOptions(argc, argv);
   Mode mode;
   try {
      const std::string rawMode = options.getCtx("mode");
      if (rawMode == "") {
         std::cerr << "Missing mode" << std::endl;
         return -1;
      }

      mode = parseMode(rawMode);
   } catch (const std::runtime_error &err) {
      std::cerr << "Exception Caught: " << err.what() << std::endl;
      return -1;
   }

   if (mode == SENDER)
      runDriver(options.getCtx("addr"), options.getCtx("filename"));
   else if (options.getCtx("port") != "") {
      unsigned int port = static_cast<unsigned int>(std::stoi(options.getCtx("port")));
      runServer(port);
   }

   return 0;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string sanitizeFilename(const std::string &input) {
   std::string name = input;
   size_t lastSlash = name.find_last_of("/\\");
   if (lastSlash != std::string::npos) 
      name = name.substr(lastSlash + 1);
   
   name = std::regex_replace(name, std::regex("\\.\\."), "");
   name = std::regex_replace(name, std::regex("[^a-zA-Z0-9._-]"), "");

   if (name.empty())
      name = "file";

   return name;
}


