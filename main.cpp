#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <regex>
#include "flags.h"
#include "mode.h"
#include "prompt.h"
#include "driver.h"
#include "server.h"

std::string sanitizeFilename(const std::string &input);
std::string trim(const std::string &str);

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

    if (selectedMode == "sender") {
        std::string addr = trim(options.getCtx("Addr"));
        std::string filename = trim(sanitizeFilename(options.getCtx("Filename")));
        runDriver(addr, filename);
    } else if (selectedMode == "receiver") {
        unsigned int port = static_cast<unsigned int>(std::stoi(options.getCtx("Port")));
        std::string outputDir = options.getCtx("OutputDir");
        runServer(port, outputDir);
    }

    return 0;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string sanitizeFilename(const std::string &input) {
    std::string name = input;
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        name = name.substr(lastSlash + 1);

    name = std::regex_replace(name, std::regex("\\.\\."), "");
    name = std::regex_replace(name, std::regex("[^a-zA-Z0-9._-]"), "");

    return name.empty() ? "file" : name;
}

