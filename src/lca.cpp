#include "lca.hpp"
#include <unordered_set>
#include <queue>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

std::string getParent(const std::string& commitHash) {
    fs::path path = ".minigit/objects/" + commitHash;
    std::ifstream in(path);
    std::string line;
    while (getline(in, line)) {
        if (line.rfind("parent:", 0) == 0) {
            return line.substr(7); // skip "parent: "
        }
    }
    return "";
}

std::unordered_set<std::string> getAncestors(const std::string& commit) {
    std::unordered_set<std::string> visited;
    std::string current = commit;
    while (!current.empty()) {
        visited.insert(current);
        current = getParent(current);
    }
    return visited;
}

std::string findLowestCommonAncestor(const std::string& commit1, const std::string& commit2) {
    auto ancestors1 = getAncestors(commit1);
    std::string current = commit2;
    while (!current.empty()) {
        if (ancestors1.count(current)) {
            return current;
        }
        current = getParent(current);
    }
    return ""; // No common ancestor found
}
