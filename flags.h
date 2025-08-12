#ifndef FLAGS_H
#define FLAGS_H

#include "options.h"

class FlagOptions : public Options {
   public:
      using Options::Options;
      void promptOptions(int argc, char *argv[]);
};

#endif
