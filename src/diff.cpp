#include "diff.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;
using namespace std;

// Reads blob content (file) line by line into a vector
vector<string> readBlobLines(const string& hash) {
    vector<string> lines;
    ifstream file(".minigit/objects/" + hash);
    if (!file) return lines;

    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

// Parses the commit file to get a map of filename -> hash
unordered_map<string, string> parseCommitFiles(const string& commitHash) {
    unordered_map<string, string> fileMap;
    ifstream file(".minigit/objects/" + commitHash);
    if (!file) return fileMap;

    string line;
    while (getline(file, line)) {
        if (line.rfind("file: ", 0) == 0) {
            stringstream ss(line.substr(6));
            string filename, hash;
            ss >> filename >> hash;
            fileMap[filename] = hash;
        }
    }

    return fileMap;
}

// Basic line-by-line diff for changed files
void diffLines(const vector<string>& lines1, const vector<string>& lines2) {
    size_t maxLines = max(lines1.size(), lines2.size());
    for (size_t i = 0; i < maxLines; ++i) {
        string a = (i < lines1.size()) ? lines1[i] : "";
        string b = (i < lines2.size()) ? lines2[i] : "";

        if (a != b) {
            if (!a.empty()) cout << "- " << a << "\n";
            if (!b.empty()) cout << "+ " << b << "\n";
        }
    }
}

// Main diff driver
void diffCommits(const string& commit1Hash, const string& commit2Hash) {
    cout << "Diffing commits:\n" << commit1Hash << "\n" << commit2Hash << "\n";

    auto files1 = parseCommitFiles(commit1Hash);
    auto files2 = parseCommitFiles(commit2Hash);

    unordered_map<string, bool> visited;

    for (const auto& [filename, hash2] : files2) {
        visited[filename] = true;

        if (files1.find(filename) == files1.end()) {
            // New file in commit2
            cout << "File added: " << filename << "\n";
            auto lines = readBlobLines(hash2);
            for (const auto& line : lines) cout << "+ " << line << "\n";
        } else {
            // File exists in both: check for changes
            const string& hash1 = files1[filename];
            if (hash1 != hash2) {
                cout << "Changes in file: " << filename << "\n";
                auto lines1 = readBlobLines(hash1);
                auto lines2 = readBlobLines(hash2);
                diffLines(lines1, lines2);
            }
        }
    }

    for (const auto& [filename, hash1] : files1) {
        if (!visited[filename]) {
            // File deleted in commit2
            cout << "File removed: " << filename << "\n";
            auto lines = readBlobLines(hash1);
            for (const auto& line : lines) cout << "- " << line << "\n";
        }
    }
}
