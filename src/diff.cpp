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

    // Collect all unique files from both commits
    std::unordered_set<std::string> allFiles;
    for (const auto& [file, _] : snap1) allFiles.insert(file);
    for (const auto& [file, _] : snap2) allFiles.insert(file);

    for (const std::string& file : allFiles) {
        bool in1 = snap1.count(file) > 0;
        bool in2 = snap2.count(file) > 0;

        if (in1 && !in2) {
            std::cout << "🗑️  " << file << " was removed in " << commit2 << "\n";
        } else if (!in1 && in2) {
            std::cout << "➕ " << file << " was added in " << commit2 << "\n";
        } else {
            // File exists in both commits
            if (snap1[file] != snap2[file]) {
                std::cout << "✏️  " << file << " changed:\n";

                fs::path blob1 = ".minigit/objects/" + snap1[file];
                fs::path blob2 = ".minigit/objects/" + snap2[file];

                std::ifstream f1(blob1), f2(blob2);
                if (!f1 || !f2) {
                    std::cerr << "Error: Unable to open blob files for " << file << "\n";
                    continue;
                }

                std::string line1, line2;
                int lineNum = 1;

                // Compare line by line until both files are fully read
                while (std::getline(f1, line1) || std::getline(f2, line2)) {
                    if (line1 != line2) {
                        std::cout << " - [" << lineNum << "] " << (line1.empty() ? "<empty>" : line1) << "\n";
                        std::cout << " + [" << lineNum << "] " << (line2.empty() ? "<empty>" : line2) << "\n";
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
