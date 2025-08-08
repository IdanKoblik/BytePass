#include "server.h"
#include "driver.h"
#include <cstring>
#include <iostream>
#include <ostream>

int main(int argc, char *argv[]) {
   std::cout << "Echo-ft" << std::endl;

   if (argc < 2) 
      return -1;

   if (std::string(argv[1]) == "server")
      runServer();
   else if (std::string(argv[2]) != "")
      runDriver(argv[2]);

   return 0;
}
