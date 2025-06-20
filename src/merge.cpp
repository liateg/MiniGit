#include "merge.hpp"
#include "utils.hpp"
#include "commit.hpp"
#include "hash.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>

using namespace std;
namespace fs = std::filesystem;

// Load a commit's snapshot: filename → blob hash
unordered_map<string, string> loadSnapshot(const string& commitHash) {
    unordered_map<string, string> snapshot;
    ifstream file(".minigit/objects/" + commitHash);
    if (!file) {
        cerr << "❌ Could not open commit " << commitHash << endl;
        return snapshot;
    }

    string line;
    bool inBlobs = false;

    while (getline(file, line)) {
        if (line == "blobs:") {
            inBlobs = true;
            continue;
        }
        if (inBlobs && !line.empty()) {
            istringstream iss(line);
            string filename, blobHash;
            if (iss >> filename >> blobHash) {
                snapshot[filename] = blobHash;
            }
        }
    }

    return snapshot;
}

// Find all ancestors of a commit
unordered_set<string> collectAncestors(const string& commitHash) {
    unordered_set<string> ancestors;
    string current = commitHash;

    while (current != string(40, '0') && !current.empty()) {
        ancestors.insert(current);
        ifstream file(".minigit/objects/" + current);
        string line;
        while (getline(file, line)) {
            if (line.rfind("parent: ", 0) == 0) {
                current = line.substr(8);
                break;
            }
        }
    }

    return ancestors;
}

// Find lowest common ancestor of two branches
string findLCA(const string& current, const string& target) {
    unordered_set<string> ancestors1 = collectAncestors(current);
    string cur = target;

    while (cur != string(40, '0') && !cur.empty()) {
        if (ancestors1.count(cur)) return cur;
        ifstream file(".minigit/objects/" + cur);
        string line;
        while (getline(file, line)) {
            if (line.rfind("parent: ", 0) == 0) {
                cur = line.substr(8);
                break;
            }
        }
    }

    return string(40, '0'); // No common ancestor
}

// Perform a basic three-way merge and show conflicts
void mergeBranch(const string& targetBranch) {
    string currentBranch = getCurrentBranch();
    string currentHash = getParentHash(currentBranch);
    string targetHash = getParentHash(targetBranch);
    string lca = findLCA(currentHash, targetHash);

    if (lca == string(40, '0')) {
        cout << "⚠️ No common ancestor found. Aborting merge.\n";
        return;
    }

    auto base = loadSnapshot(lca);
    auto current = loadSnapshot(currentHash);
    auto target = loadSnapshot(targetHash);

    unordered_set<string> allFiles;
    for (auto& [f, _] : base) allFiles.insert(f);
    for (auto& [f, _] : current) allFiles.insert(f);
    for (auto& [f, _] : target) allFiles.insert(f);

    for (const string& file : allFiles) {
        string baseHash = base[file];
        string currHash = current[file];
        string targHash = target[file];

        if (currHash == targHash) continue; // No change
        if (currHash == baseHash) {
            // Updated in target, apply change
            string blobPath = ".minigit/objects/" + targHash;
            ifstream in(blobPath);
            ofstream out(file);
            out << in.rdbuf();
            cout << "✅ Merged updated file: " << file << "\n";
        } else if (targHash == baseHash) {
            // Modified only in current branch → do nothing
        } else {
            // Conflict
            cout << "⚠️ CONFLICT: both modified " << file << "\n";
            ofstream out(file);
            out << "<<<<<<< HEAD\n";
            if (fs::exists(".minigit/objects/" + currHash)) {
                ifstream c(".minigit/objects/" + currHash);
                out << c.rdbuf();
            }
            out << "=======\n";
            if (fs::exists(".minigit/objects/" + targHash)) {
                ifstream t(".minigit/objects/" + targHash);
                out << t.rdbuf();
            }
            out << ">>>>>>>\n";
        }
    }

    cout << "✅ Merge completed (with possible conflicts).\n";
}
