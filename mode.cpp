#include <algorithm>
#include <stdexcept>
#include "mode.h"

Mode parseMode(std::string value) {
   std::transform(value.begin(), value.end(), value.begin(), ::tolower);
   if (value == "sender")
      return SENDER;

   if (value == "receiver") 
      return RECEIVER;

   throw std::runtime_error("Invalid mode.");
}
