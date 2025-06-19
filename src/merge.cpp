#include "../include/merge.hpp"
#include "commit.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <unordered_set>
#include <map>
#include <fstream>
#include <sstream>

void collectAncestors(CommitNode* node, std::unordered_set<std::string>& visited) {
    while (node) {
        visited.insert(node->id);
        node = node->parent;
    }
}

CommitNode* findLowestCommonAncestor(CommitNode* a, CommitNode* b) {
    std::unordered_set<std::string> visited;
    collectAncestors(a, visited);
    while (b) {
        if (visited.count(b->id)) return b;
        b = b->parent;
    }
    return nullptr;
}

void mergeBranches(CommitNode* current, CommitNode* target) {
    CommitNode* lca = findLowestCommonAncestor(current, target);
    if (!lca) {
        std::cout << "Error: No common ancestor found.\n";
        return;
    }

    std::map<std::string, std::string> mergedFiles;

    for (const auto& [file, baseHash] : lca->files) {
        std::string currentHash = current->files.count(file) ? current->files.at(file) : "";
        std::string targetHash  = target->files.count(file) ? target->files.at(file)  : "";

        if (currentHash == baseHash && targetHash != baseHash) {
            mergedFiles[file] = targetHash;
        } else if (targetHash == baseHash && currentHash != baseHash) {
            mergedFiles[file] = currentHash;
        } else if (currentHash == targetHash) {
            mergedFiles[file] = currentHash;
        } else {
            std::cout << "CONFLICT: both modified " << file << "\n";
            mergedFiles[file] = "CONFLICT";
        }
    }

    for (const auto& [file, hash] : current->files)
        if (!lca->files.count(file)) mergedFiles[file] = hash;

    for (const auto& [file, hash] : target->files)
        if (!lca->files.count(file) && !current->files.count(file))
            mergedFiles[file] = hash;

    std::cout << "\nMerge Result Preview:\n";
    for (const auto& [file, hash] : mergedFiles) {
        std::cout << file << ": " << hash << "\n";
    }
}

void showDiff(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1);
    std::ifstream f2(file2);

    if (!f1.is_open() || !f2.is_open()) {
        std::cout << "Error opening one or both files for diff.\n";
        return;
    }

    std::string line1, line2;
    int lineNum = 1;

    while (std::getline(f1, line1) && std::getline(f2, line2)) {
        if (line1 != line2) {
            std::cout << "Line " << lineNum << " differs:\n";
            std::cout << "\t" << file1 << ": " << line1 << "\n";
            std::cout << "\t" << file2 << ": " << line2 << "\n";
        }
        ++lineNum;
    }

    while (std::getline(f1, line1)) {
        std::cout << "Line " << lineNum << " only in " << file1 << ": " << line1 << "\n";
        ++lineNum;
    }

    while (std::getline(f2, line2)) {
        std::cout << "Line " << lineNum << " only in " << file2 << ": " << line2 << "\n";
        ++lineNum;
    }

    f1.close();
    f2.close();
}
