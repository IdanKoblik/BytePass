#include <string>
#include <vector>
#include <limits>
#include "prompt.h"

void PromptOptions::promptOptions(int argc, char *argv[]) {
   const std::vector<std::string> &options = this->getOptions();

   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   for (const std::string& option : options) {
      std::string value;
      std::cout << option << ": " << std::endl;
      std::getline(std::cin, value);

      this->addCtx(option, value);
   }
}
