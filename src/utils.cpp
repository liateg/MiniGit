#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

// Reads commit snapshot mapping from commit object file
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

// Returns the current branch name by reading .minigit/HEAD
std::string getCurrentBranch() {
    std::ifstream head(".minigit/HEAD");
    if (!head) {
        std::cerr << "Error: Could not open .minigit/HEAD\n";
        return "";
    }

    std::string line;
    std::getline(head, line);  // e.g., "ref: refs/heads/main"

    size_t lastSlash = line.rfind('/');
    if (lastSlash != std::string::npos && lastSlash + 1 < line.size()) {
        return line.substr(lastSlash + 1);  // e.g., "main"
    }

    return "";
}

// Returns the parent commit hash for the given branch, or 40 zero chars if none found
std::string getParentHash(const std::string& cbranch) {
    std::ifstream branch(".minigit/refs/heads/" + cbranch);
    if (!branch) {
        std::cerr << "Error: Could not open .minigit/refs/heads/" << cbranch << "\n";
        return std::string(40, '0');
    }

    std::string line;
    std::getline(branch, line);

    if (line.empty()) {
        return std::string(40, '0');  // Empty means no parent commit
    }
    return line;
}
