#include <exception>
#include "commit.hpp"
#include "add.hpp"       // for IndexEntry and index parsing
#include "hash.hpp"      
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;
using namespace std;

<<<<<<< HEAD
// Get current timestamp as string (without newline)-use it for hashing
=======
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    string ts = ctime(&now_c);
    if (!ts.empty() && ts.back() == '\n') {
        ts.pop_back();
    }
    return ts;
}

// Create an empty commit object file
void createCommitObject(const string& commitHash) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath);
    if (!commitFile) {
        cerr << "Error: Could not create commit object file.\n";
    }
}


void writeCommitMetadata(const string& commitHash, const string& message,
                         const string& parentHash, const string& timestamp) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);
    if (!commitFile) {
        cerr << "Error: Could not open commit file for writing.\n";
        return;
    }

    commitFile << "commit: " << commitHash << "\n";
    commitFile << "parent: " << parentHash << "\n";
    commitFile << "timestamp: " << timestamp << "\n";
    commitFile << "message: " << message << "\n";
    commitFile << "blobs:\n";
}

void writeCommitSnapshot(const string& commitHash,
                         const unordered_map<string, IndexEntry>& indexMap) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);
    if (!commitFile) {
        cerr << "Error: Could not open commit file to write snapshot.\n";
        return;
    }

    for (const auto& [filename, entry] : indexMap) {
        if (!entry.stagedForRemoval) {
            commitFile << filename << " " << entry.lastCommitHash << "\n";
        }
    }
}

void updateBranchRef(const string& commitHash) {
    string branch = getCurrentBranch();
    ofstream branchFile(".minigit/refs/heads/" + branch);
    if (!branchFile) {
        cerr << "Error: Could not update branch pointer.\n";
        return;
    }
    branchFile << commitHash;
}

// Main commit creation logic
void createcommit(const string& message,
                  unordered_map<string, IndexEntry>& indexMap) {
    string timestamp = getCurrentTimestamp();
    string branch = getCurrentBranch();
    string parentHash = getParentHash(branch);

    unordered_map<string, string> snapshot;  // filename → blob hash
    string totalContent;
    bool hasRealChanges = false;

    
    for (auto& [filename, entry] : indexMap) {
        if (entry.stagedForRemoval) {
            hasRealChanges = true;
            continue; 
        }

        ifstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file '" << filename << "'\n";
            continue;
        }

        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        string blobHash = computeHash(content);
        snapshot[filename] = blobHash;

        if (blobHash != entry.lastCommitHash) {
            hasRealChanges = true;
            entry.lastCommitHash = blobHash;

            
            fs::path blobPath = ".minigit/objects/" + blobHash;
            if (!fs::exists(blobPath)) {
                ofstream out(blobPath);
                out << content;
            }
        }

        totalContent += filename + content;
    }

    
    if (!hasRealChanges) {
        cout << "No changes to commit.\n";
        return;
    }

    
    totalContent += message + timestamp + parentHash + to_string(chrono::system_clock::now().time_since_epoch().count());
    string commitHash = computeHash(totalContent);

    
    createCommitObject(commitHash);
    writeCommitMetadata(commitHash, message, parentHash, timestamp);

    
    writeCommitSnapshot(commitHash, indexMap);

    
    updateBranchRef(commitHash);
    writeIndex(indexMap);

    cout << "Commit created: " << commitHash << "\n";
}
