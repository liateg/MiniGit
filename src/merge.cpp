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

unordered_map<string, string> loadSnapshot(const string& commitHash) {
    unordered_map<string, string> snapshot;
    ifstream file(".minigit/objects/" + commitHash);
    if (!file) {
        cerr << "Could not open commit " << commitHash << endl;
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

unordered_set<string> collectAncestors(const string& commitHash) {
    unordered_set<string> ancestors;
    string current = commitHash;

    while (current != string(40, '0') && !current.empty()) {
        ancestors.insert(current);
        ifstream file(".minigit/objects/" + current);
        if (!file) break;

        bool foundParent = false;
        string line;
        while (getline(file, line)) {
            if (line.rfind("parent: ", 0) == 0) {
                current = line.substr(8);
                foundParent = true;
                break;
            }
        }
        if (!foundParent) break;
    }

    return ancestors;
}

string findLCA(const string& current, const string& target) {
    unordered_set<string> ancestors1 = collectAncestors(current);
    string cur = target;

    while (cur != string(40, '0') && !cur.empty()) {
        if (ancestors1.count(cur)) return cur;
        ifstream file(".minigit/objects/" + cur);
        if (!file) break;

        bool foundParent = false;
        string line;
        while (getline(file, line)) {
            if (line.rfind("parent: ", 0) == 0) {
                cur = line.substr(8);
                foundParent = true;
                break;
            }
        }
        if (!foundParent) break;
    }

    return string(40, '0'); 
}

void mergeBranch(const string& targetBranch) {
    string currentBranch = getCurrentBranch();
    string currentHash = getParentHash(currentBranch);
    string targetHash = getParentHash(targetBranch);
    string lca = findLCA(currentHash, targetHash);

    if (lca == string(40, '0')) {
        cout << "No common ancestor found. Aborting merge.\n";
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
        string baseHash = base.count(file) ? base[file] : "";
        string currHash = current.count(file) ? current[file] : "";
        string targHash = target.count(file) ? target[file] : "";

        if (currHash == targHash) continue;

        if (currHash == baseHash) {

            fs::path blobPath = ".minigit/objects/" + targHash;
            ifstream in(blobPath);
            if (!in) {
                cerr << "Error: Could not open blob " << targHash << " for file " << file << "\n";
                continue;
            }
            ofstream out(file);
            out << in.rdbuf();
            cout << "Merged updated file: " << file << "\n";
        } else if (targHash == baseHash) {
            
        } else {
            
            cout << "CONFLICT: both modified " << file << "\n";

            ofstream out(file);
            out << "<<<<<<< " << currentBranch << "\n";
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

    cout << "Merge completed (with possible conflicts).\n";
}
