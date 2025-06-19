#include "diff.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
using namespace std;

// Read the snapshot section of a commit and return map of filename -> blob hash
unordered_map<string, string> readCommitSnapshot(const string& commitHash) {
    unordered_map<string, string> snapshot;
    ifstream file(".minigit/objects/" + commitHash);
    if (!file) {
        cerr << "Error: Could not open commit object: " << commitHash << endl;
        return snapshot;
    }

    string line;
    bool inBlobsSection = false;
    while (getline(file, line)) {
        if (line == "blobs:") {
            inBlobsSection = true;
            continue;
        }
        if (inBlobsSection && !line.empty()) {
            istringstream iss(line);
            string filename, blobHash;
            iss >> filename >> blobHash;
            snapshot[filename] = blobHash;
        }
    }
    return snapshot;
}

// Read blob content from .minigit/objects/<blobHash>
vector<string> readBlobLines(const string& blobHash) {
    vector<string> lines;
    ifstream blobFile(".minigit/objects/" + blobHash);
    if (!blobFile) return lines;

    string line;
    while (getline(blobFile, line)) {
        lines.push_back(line);
    }
    return lines;
}

void diffCommits(const string& commit1, const string& commit2) {
    unordered_map<string, string> snapshot1 = readCommitSnapshot(commit1);
    unordered_map<string, string> snapshot2 = readCommitSnapshot(commit2);

    cout << "Comparing commit " << commit1 << " with " << commit2 << "...\n";

    unordered_map<string, bool> printed;

    // Check for modified or removed files
    for (const auto& [filename, hash1] : snapshot1) {
        printed[filename] = true;

        if (snapshot2.find(filename) == snapshot2.end()) {
            cout << "File removed: " << filename << "\n";
            continue;
        }

        string hash2 = snapshot2[filename];
        if (hash1 != hash2) {
            cout << "Modified file: " << filename << "\n";

            vector<string> lines1 = readBlobLines(hash1);
            vector<string> lines2 = readBlobLines(hash2);

            size_t maxLines = max(lines1.size(), lines2.size());
            for (size_t i = 0; i < maxLines; ++i) {
                string l1 = (i < lines1.size()) ? lines1[i] : "";
                string l2 = (i < lines2.size()) ? lines2[i] : "";

                if (l1 != l2) {
                    cout << " - " << l1 << "\n";
                    cout << " + " << l2 << "\n";
                }
            }
        }
    }

    // Check for newly added files
    for (const auto& [filename, hash2] : snapshot2) {
        if (printed.find(filename) == printed.end()) {
            cout << "New file added: " << filename << "\n";

            vector<string> lines = readBlobLines(hash2);
            for (const auto& line : lines) {
                cout << " + " << line << "\n";
            }
        }
    }
}
