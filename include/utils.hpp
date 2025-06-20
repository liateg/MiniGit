#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <unordered_map>

std::string getCurrentBranch();

std::string getParentHash(const std::string& cbranch);

// Returns a map of filename to hash for the given commit
std::unordered_map<std::string, std::string> getCommitSnapshot(const std::string& commitHash);

#endif // UTILS_HPP
