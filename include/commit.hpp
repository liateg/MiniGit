#ifndef COMMIT_HPP
#define COMMIT_HPP

#include <string>
#include <unordered_map>
#include "add.hpp"  // For IndexEntry

// Define for future use: log, checkout, merge
struct Commit {
    std::string hash;
    std::string parentHash;
    std::string timestamp;
    std::string message;
    std::unordered_map<std::string, std::string> fileMap;  // filename -> blob hash
};

// Function declarations
std::string getCurrentTimestamp();
std::string getCurrentBranch();
std::string getParentHash(const std::string& branch);

void createCommitObject(const std::string& commitHash);
void writeCommitMetadata(const std::string& commitHash, const std::string& message,
                         const std::string& parentHash, const std::string& timestamp);
void writeCommitSnapshot(const std::string& commitHash,
                         const std::unordered_map<std::string, IndexEntry>& indexMap);
void updateBranchRef(const std::string& commitHash);

void createcommit(const std::string& message,
                  std::unordered_map<std::string, IndexEntry>& indexMap);

#endif  // COMMIT_HPP
