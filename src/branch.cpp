#include <iostream>
#include <fstream>
#include <filesystem>
#include "utils.hpp"
#include "branch.hpp"
namespace fs = std::filesystem;

void createBranch(const std::string& branchName) {
    fs::path branchPath = ".minigit/refs/heads/" + branchName;

    if (fs::exists(branchPath)) {
        std::cerr << "Error: Branch '" << branchName << "' already exists.\n";
        return;
    }

    std::string currentBranch = getCurrentBranch();
    std::ifstream headFile(".minigit/refs/heads/" + currentBranch);
    std::string currentHash;
    getline(headFile, currentHash);

    std::ofstream newBranchFile(branchPath);
    newBranchFile << currentHash;

    std::cout << "✅ Branch '" << branchName << "' created.\n";
}
