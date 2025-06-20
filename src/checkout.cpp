#include "checkout.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;

// Helper: load snapshot from a commit object
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

// Checkout logic
void checkout(const string& target) {
    string commitHash;
    fs::path branchPath = ".minigit/refs/heads/" + target;

    if (fs::exists(branchPath)) {
        // It's a branch, get the latest commit from it
        ifstream in(branchPath);
        getline(in, commitHash);
        in.close();

        // Update HEAD to this branch
        ofstream head(".minigit/HEAD");
        head << "ref: refs/heads/" << target;
        head.close();
        cout << "🔀 Switched to branch: " << target << endl;
    } else {
        // Assume it's a direct commit hash
        commitHash = target;

        // Update HEAD to detached
        ofstream head(".minigit/HEAD");
        head << commitHash;
        head.close();
        cout << "🔀 Detached HEAD at: " << commitHash << endl;
    }

    // Load snapshot and apply to working dir
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
        blob.close();
        outFile.close();
    }

    cout << "✅ Files updated to match commit " << commitHash << endl;
}
