#include <exception>

#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>


std::unordered_map<std::string, std::string> getCommitSnapshot(const std::string& commitHash) {
    std::unordered_map<std::string, std::string> snapshot;
    std::ifstream file(".minigit/objects/" + commitHash);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open commit file " << commitHash << "\n";
        return snapshot;
    }

    std::string line;
    bool blobsSection = false;

    while (std::getline(file, line)) {
        if (line == "blobs:") {
            blobsSection = true;
            continue;
        }

        if (blobsSection && !line.empty()) {
            std::istringstream iss(line);
            std::string filename, blobHash;
            if (iss >> filename >> blobHash) {
                snapshot[filename] = blobHash;
            }
        }
    }

    return snapshot;
}

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