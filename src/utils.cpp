#include <exception>

#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
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
// Get the parent commit hash from the current branch
string getParentHash(const string& cbranch) {
    ifstream branch(".minigit/refs/heads/" + cbranch);
    if (!branch) {
        cerr << "Error: Could not open .minigit/refs/heads/" << cbranch << "\n";
        return string(40, '0');
    }

    string line;
    getline(branch, line); 
    branch.close();

    if (line.empty()) {
        return string(40, '0');  // 40-char zero string all 0, the hash is 40 chars so when its empty its 40 os
    } else {
        return line;
    }
}