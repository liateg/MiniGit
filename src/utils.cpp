#include "utils.hpp"
#include <fstream>
#include <iostream>

std::string getCurrentBranch() {
    std::ifstream head(".minigit/HEAD");
    if (!head) {
        std::cerr << "Error: Could not open .minigit/HEAD\n";
        return "";
    }

    std::string line;
    std::getline(head, line);  // e.g., "ref: refs/heads/main"
    head.close();

    size_t lastSlash = line.rfind('/');
    if (lastSlash != std::string::npos && lastSlash + 1 < line.size()) {
        return line.substr(lastSlash + 1);  // "main"
    }

    return "";
}
