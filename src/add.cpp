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
//Check if the file exists and have correct extention
bool isValidFile(const string& filename) {
    return fs::exists(filename) && fs::is_regular_file(filename);
}

//Read the the whole file
string readFileContent(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return "";
    }
    stringstream buffer; //not line by line 
    buffer << file.rdbuf();
    return buffer.str();
}

//Parse the staged file into a map, will use in commit as well
unordered_map<string, IndexEntry> parseIndex() {
    unordered_map<string, IndexEntry> indexMap; //empty map
    ifstream indexFile(".minigit/index");
    if (!indexFile) return indexMap; //Nothing is staged

    string filename, hash, stagedFlag, branch;
    while (indexFile >> filename >> hash >> stagedFlag >> branch) {
        IndexEntry entry;  //this is a struct from the header
        entry.lastCommitHash = hash;
        entry.stagedForRemoval = (stagedFlag == "1");
        entry.branchName = branch; 
        indexMap[filename] = entry; //mape the filename -> struct
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
    if (!fs::exists(objectsDir)) fs::create_directories(objectsDir); //chek if it hasnt been created in the init,just to be safe

    fs::path destPath = objectsDir / hash;
    fs::copy_file(filename, destPath, fs::copy_options::overwrite_existing); //store the snapshot

    auto indexMap = parseIndex();

    if (indexMap.find(filename) != indexMap.end() &&
        indexMap[filename].lastCommitHash == hash && //if no change
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
