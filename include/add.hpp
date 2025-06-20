#ifndef ADD_HPP
#define ADD_HPP
#include <string>
#include <unordered_map>

struct IndexEntry {
    std::string lastCommitHash;
    bool stagedForRemoval;
    std::string branchName;
    std::string filename;
    std::string currentHash;
};

bool isValidFile(const std::string& filename);
std::string readFileContent(const std::string& filename);
std::unordered_map<std::string, IndexEntry> parseIndex();
void writeIndex(const std::unordered_map<std::string, IndexEntry>& indexMap);
void addFileToStaging(const std::string& filename);

#endif
