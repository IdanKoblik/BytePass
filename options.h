#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>
#include <map>
#include <regex>
#include <vector>

class Options {
   private:
      std::vector<std::string> options;
      std::map<std::string, std::string> ctx;

   public:
      Options(const std::vector<std::string> &initialOptions = {}) : options(initialOptions) {}

      ~Options() = default;

      void addCtx(const std::string &key, const std::string &value) {
         ctx[key] = value;
      }

      bool removeCtx(const std::string &key) {
         return ctx.erase(key) > 0;
      }

      std::string getCtx(const std::string &key) const {
         auto value = ctx.find(key);
         if (value != ctx.end())
            return value->second;

         return "";
      }

      const std::vector<std::string>& getOptions() const {
         return options;
      }

      virtual void promptOptions(int argc, char *argv[]) = 0;
};

#endif
