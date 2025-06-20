#include "checkout.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;

/**
 * Loads a commit's file-to-hash mapping from the object database
 * @param commitHash The hash of the commit to load
 * @return unordered_map mapping filenames to their blob hashes in the commit
 */
unordered_map<string, string> loadCommitSnapshot(const string& commitHash) {
    unordered_map<string, string> snapshot;
    
    // Path to commit object in minigit's object database
    fs::path commitPath = ".minigit/objects/" + commitHash;
    
    // Open commit object file
    ifstream in(commitPath);
    if (!in) {
        cerr << "Error: Commit " << commitHash << " not found.\n";
        return snapshot;
    }

    string line;
    bool readingBlobs = false;
    
    // Parse commit file line by line
    while (getline(in, line)) {
        // The "blobs:" marker indicates start of file-hash mappings
        if (line == "blobs:") {
            readingBlobs = true;
            continue;
        }

        // After "blobs:", each line contains a filename and its hash
        if (readingBlobs) {
            stringstream ss(line);
            string filename, blobHash;
            ss >> filename >> blobHash;
            
            // Store valid filename-hash pairs in the snapshot
            if (!filename.empty() && !blobHash.empty()) {
                snapshot[filename] = blobHash;
            }
        }
    }

    return snapshot;
}

<<<<<<< HEAD
/**
 * Checks out a branch or commit, updating HEAD and working directory files
 * @param target Either a branch name or commit hash to check out
 */
=======

>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
void checkout(const string& target) {
    string commitHash;
    fs::path branchPath = ".minigit/refs/heads/" + target;

    // Case 1: Checking out a branch
    if (fs::exists(branchPath)) {
<<<<<<< HEAD
        // Read the commit hash that the branch points to
=======
        
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
        ifstream in(branchPath);
        if (!in) {
            cerr << "Error: Unable to read branch file " << branchPath << endl;
            return;
        }
        getline(in, commitHash);
        in.close();

<<<<<<< HEAD
        // Update HEAD to point to this branch (normal branch checkout)
=======
        
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
        ofstream head(".minigit/HEAD");
        if (!head) {
            cerr << "Error: Unable to update HEAD.\n";
            return;
        }
        head << "ref: refs/heads/" << target;
        head.close();
        cout << "Switched to branch: " << target << endl;
<<<<<<< HEAD
    } 
    // Case 2: Checking out a specific commit (detached HEAD)
    else {
=======
    } else {
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
        commitHash = target;

        // Update HEAD directly with commit hash (detached HEAD state)
        ofstream head(".minigit/HEAD");
        if (!head) {
            cerr << "Error: Unable to update HEAD.\n";
            return;
        }
        head << commitHash;
        head.close();
        cout << "Detached HEAD at: " << commitHash << endl;
    }

<<<<<<< HEAD
    // Restore working directory files to match the target commit
=======
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
    auto snapshot = loadCommitSnapshot(commitHash);
    for (const auto& [filename, blobHash] : snapshot) {
        // Path to blob in object database
        fs::path blobPath = ".minigit/objects/" + blobHash;
        
        // Open blob containing file content
        ifstream blob(blobPath);
        if (!blob) {
            cerr << "Missing blob: " << blobHash << endl;
            continue;
        }

        // Overwrite working directory file with blob content
        ofstream outFile(filename);
        outFile << blob.rdbuf();
    }

    cout << "Files updated to match commit " << commitHash << endl;
<<<<<<< HEAD
}
=======
}
>>>>>>> eaa5a60ceb077d32ade51193906c552139a2ef50
