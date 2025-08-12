#ifndef MODE_H
#define MODE_H

#include <string>
typedef enum {
   SENDER,
   RECEIVER
} Mode;

Mode parseMode(const std::string value);

#endif
