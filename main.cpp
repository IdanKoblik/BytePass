#include "server.h"
#include "driver.h"
#include <cstring>
#include <iostream>
#include <ostream>

int main(int argc, char *argv[]) {
   std::cout << "Echo-ft" << std::endl;

   if (argc > 1 && std::string(argv[1]) == "server")
      runServer();
   else
      runDriver();

   return 0;
}
