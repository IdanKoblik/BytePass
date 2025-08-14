#ifndef PROMPT_H
#define PROMPT_H

#include "options.h"

class PromptOptions : public Options {
   public:
      using Options::Options;
      void promptOptions(int argc, char *argv[]);
};

#endif
