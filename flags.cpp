#include <string>
#include "flags.h"

void FlagOptions::promptOptions(int argc, char *argv[]) {
   const std::vector<std::string> &options = this->getOptions();
   for (const std::string &option : options) {
      for (int i = 0; i < argc; i++) {
         const std::string arg = argv[i];
         std::string prefix = "--" + option + "=";

         if ((arg.find(prefix) == 0) && (this->getCtx(option.c_str()) == ""))
            this->addCtx(option, arg.substr(prefix.length()));
      }
   }
}
