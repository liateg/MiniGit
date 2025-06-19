#ifndef ADD_HPP
#define ADD_HPP

#include <string>
#include <unordered_map>

// Struct for index entry with new fields
struct IndexEntry {
    std::string lastCommitHash;      // Hash from the last commit
    bool stagedForRemoval;           // True if marked for deletion
    std::string branchName;          // Name of the branch where last committed
    std::string filename;            // Name of the file
    std::string currentHash;         // Hash of the latest staged version (used in commits)
};


// Function declarations
bool isValidFile(const std::string& filename);
std::string readFileContent(const std::string& filename);
std::unordered_map<std::string, IndexEntry> parseIndex();
void writeIndex(const std::unordered_map<std::string, IndexEntry>& indexMap);
void addFileToStaging(const std::string& filename);

#endif
