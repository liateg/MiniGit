#include "diff.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

void diffCommits(const std::string& commit1, const std::string& commit2) {
    auto snap1 = getCommitSnapshot(commit1);
    auto snap2 = getCommitSnapshot(commit2);

    std::unordered_set<std::string> allFiles;
    for (const auto& [f, _] : snap1) allFiles.insert(f);
    for (const auto& [f, _] : snap2) allFiles.insert(f);

    for (const std::string& file : allFiles) {
        bool in1 = snap1.count(file);
        bool in2 = snap2.count(file);

        if (in1 && !in2) {
            std::cout << "🗑️  " << file << " was removed in " << commit2 << "\n";
        } else if (!in1 && in2) {
            std::cout << "➕ " << file << " was added in " << commit2 << "\n";
        } else {
            if (snap1[file] != snap2[file]) {
                std::cout << "✏️  " << file << " changed:\n";

                fs::path blob1 = ".minigit/objects/" + snap1[file];
                fs::path blob2 = ".minigit/objects/" + snap2[file];

                std::ifstream f1(blob1), f2(blob2);
                std::string line1, line2;
                int lineNum = 1;

                while (std::getline(f1, line1) || std::getline(f2, line2)) {
                    if (line1 != line2) {
                        std::cout << " - [" << lineNum << "] " << line1 << "\n";
                        std::cout << " + [" << lineNum << "] " << line2 << "\n";
                    }
                    line1.clear();
                    line2.clear();
                    ++lineNum;
                }

                std::cout << "\n";
            }
        }
    }
}
