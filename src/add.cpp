#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include "add.hpp"
#include "hash.hpp"
#include "utils.hpp"  // for getCurrentBranch()

namespace fs = std::filesystem;
using namespace std;

bool isValidFile(const string& filename) {
    return fs::exists(filename) && fs::is_regular_file(filename);
}

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

unordered_map<string, IndexEntry> parseIndex() {
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

void writeIndex(const unordered_map<string, IndexEntry>& indexMap) {
    ofstream indexFile(".minigit/index", ios::trunc);
    for (const auto& [filename, entry] : indexMap) {
        indexFile << filename << " "
                  << entry.lastCommitHash << " "
                  << (entry.stagedForRemoval ? "1" : "0") << " "
                  << entry.branchName << "\n";
    }
}

void addFileToStaging(const string& filename) {
    if (!isValidFile(filename)) {
        cerr << "Error: File does not exist or is not valid.\n";
        return;
    }

    string hash = computeHash(filename);
    fs::path objectsDir = ".minigit/objects";
    if (!fs::exists(objectsDir)) fs::create_directories(objectsDir);

    fs::path destPath = objectsDir / hash;
    fs::copy_file(filename, destPath, fs::copy_options::overwrite_existing);

    auto indexMap = parseIndex();

    if (indexMap.find(filename) != indexMap.end() &&
        indexMap[filename].lastCommitHash == hash &&
        !indexMap[filename].stagedForRemoval) {
        cout << "File '" << filename << "' is already staged with no changes.\n";
        return;
    }

    IndexEntry newEntry;
    newEntry.lastCommitHash = hash;
    newEntry.stagedForRemoval = false;
    newEntry.branchName = getCurrentBranch();
  
    indexMap[filename] = newEntry;

    writeIndex(indexMap);
    cout << "File '" << filename << "' added to staging area.\n";
}
