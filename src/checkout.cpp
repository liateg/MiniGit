#include "checkout.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;

// Load file-to-hash snapshot from a commit object file
unordered_map<string, string> loadCommitSnapshot(const string& commitHash) {
    unordered_map<string, string> snapshot;
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ifstream in(commitPath);
    if (!in) {
        cerr << "Error: Commit " << commitHash << " not found.\n";
        return snapshot;
    }

    string line;
    bool readingBlobs = false;
    while (getline(in, line)) {
        if (line == "blobs:") {
            readingBlobs = true;
            continue;
        }

        if (readingBlobs) {
            stringstream ss(line);
            string filename, blobHash;
            ss >> filename >> blobHash;
            if (!filename.empty() && !blobHash.empty()) {
                snapshot[filename] = blobHash;
            }
        }
    }

    return snapshot;
}

// Checkout to a branch or commit hash
void checkout(const string& target) {
    string commitHash;
    fs::path branchPath = ".minigit/refs/heads/" + target;

    if (fs::exists(branchPath)) {
        // Target is a branch
        ifstream in(branchPath);
        if (!in) {
            cerr << "Error: Unable to read branch file " << branchPath << endl;
            return;
        }
        getline(in, commitHash);
        in.close();

        // Update HEAD to point to this branch
        ofstream head(".minigit/HEAD");
        if (!head) {
            cerr << "Error: Unable to update HEAD.\n";
            return;
        }
        head << "ref: refs/heads/" << target;
        head.close();
        cout << "🔀 Switched to branch: " << target << endl;
    } else {
        // Treat target as commit hash (detached HEAD)
        commitHash = target;

        ofstream head(".minigit/HEAD");
        if (!head) {
            cerr << "Error: Unable to update HEAD.\n";
            return;
        }
        head << commitHash;
        head.close();
        cout << "🔀 Detached HEAD at: " << commitHash << endl;
    }

    // Load commit snapshot and overwrite files
    auto snapshot = loadCommitSnapshot(commitHash);
    for (const auto& [filename, blobHash] : snapshot) {
        fs::path blobPath = ".minigit/objects/" + blobHash;
        ifstream blob(blobPath);
        if (!blob) {
            cerr << "Missing blob: " << blobHash << endl;
            continue;
        }

        ofstream outFile(filename);
        outFile << blob.rdbuf();
    }

    cout << "✅ Files updated to match commit " << commitHash << endl;
}
