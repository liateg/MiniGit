#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include "add.hpp"
#include "hash.hpp"

namespace fs = std::filesystem;
using namespace std;

// Check if file is valid and regular(correct extension)
bool isValidFile(const string& filename) {
    return fs::exists(filename) && fs::is_regular_file(filename);
}

// Read entire content of file
string readFileContent(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Parse the index into a hashmap
unordered_map<string, IndexEntry> parseIndex() {// IndexEntry is defiend on the header
    unordered_map<string, IndexEntry> indexMap;
    ifstream indexFile(".minigit/index");
    if (!indexFile) return indexMap;

    string filename, hash, stagedFlag, branch;
    while (indexFile >> filename >> hash >> stagedFlag >> branch) {
        IndexEntry entry;
        entry.lastCommitHash = hash;
        entry.stagedForRemoval = (stagedFlag == "1");
        entry.branchName = branch; 
        indexMap[filename] = entry;
    }

    return indexMap;
}

// Write updated index map to file
void writeIndex(const unordered_map<string, IndexEntry>& indexMap) {
    ofstream indexFile(".minigit/index", ios::trunc);
    for (const auto& [filename, entry] : indexMap) {
        indexFile << filename << " "
                  << entry.lastCommitHash << " "
                  << (entry.stagedForRemoval ? "1" : "0") << " "
                  << entry.branchName << "\n";
    }
}

// Add file to staging area
void addFileToStaging(const string& filename) {
    if (!isValidFile(filename)) {
        cerr << "Error: File does not exist or is not valid.\n";
        return;
    }

    string hash = computeHash(filename);
    fs::path objectsDir = ".minigit/objects";
    if (!fs::exists(objectsDir)) fs::create_directories(objectsDir);
//save the file and its has in the hased dir
    fs::path destPath = objectsDir / hash;
    fs::copy_file(filename, destPath, fs::copy_options::overwrite_existing);
//get the data in the index file as a map
    auto indexMap = parseIndex();

    // Avoid re-staging identical content
    if (indexMap.find(filename) != indexMap.end() &&
        indexMap[filename].lastCommitHash == hash &&
        !indexMap[filename].stagedForRemoval) {
        cout << "File '" << filename << "' is already staged with no changes.\n";
        return;
    }

    // Add or update
    IndexEntry newEntry;
    newEntry.lastCommitHash = hash;
    newEntry.stagedForRemoval = false;
    newEntry.branchName = "main";  // Default for now
    indexMap[filename] = newEntry;

    writeIndex(indexMap);
    cout << "File '" << filename << "' added to staging area.\n";
}
