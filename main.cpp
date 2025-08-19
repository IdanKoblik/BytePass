#include <iostream>
#include <map>
#include <vector>
#include "flags.h"
#include "prompt.h"
#include "driver.h"
#include "server.h"
#include "utils.h"
#include "mode.h"

int main(int argc, char *argv[]) {
    std::map<std::string, std::vector<std::string>> modeOptions = {
        {"sender", {"Addr", "Filename"}},
        {"receiver", {"Port", "OutputDir"}}
    };

    std::string selectedMode;
    if (argc == 1) {
        std::cout << "=== Select mode ===" << std::endl;
        unsigned int idx = 1;
        for (const auto &kv : modeOptions)
            std::cout << idx++ << ". " << kv.first << std::endl;

        std::cout << ">>> ";
        unsigned int choice;
        std::cin >> choice;

        if (choice < 1 || choice > modeOptions.size()) {
            std::cerr << "Invalid choice" << std::endl;
            return -1;
        }

        auto it = modeOptions.begin();
        std::advance(it, choice - 1);
        selectedMode = it->first;
    } else {
        FlagOptions options({"mode"});
        options.promptOptions(argc, argv);
        selectedMode = trim(options.getCtx("mode"));
        if (selectedMode.empty() || modeOptions.find(selectedMode) == modeOptions.end()) {
            std::cerr << "Invalid or missing mode" << std::endl;
            return -1;
        }
    }

    PromptOptions options(modeOptions[selectedMode]);
    options.promptOptions(argc, argv);

    if (parseMode(selectedMode) == SENDER) {
        std::string addr = trim(options.getCtx("Addr"));
        std::string filename = trim(sanitizeFilename(options.getCtx("Filename")));
        runDriver(addr, filename);
    } else if (parseMode(selectedMode) == RECEIVER) {
        unsigned int port = static_cast<unsigned int>(std::stoi(options.getCtx("Port")));
        std::string outputDir = options.getCtx("OutputDir");
        runServer(port, outputDir);
    }

    return 0;
}


