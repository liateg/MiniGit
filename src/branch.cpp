
#include <fstream>
#include <iostream>
#include <string>
#include "branch.hpp"
void createBranch(const std::string& branchName) {
    std::ifstream headFile(".minigit/HEAD");
    std::string currentCommit;
    std::getline(headFile, currentCommit); 
    headFile.close();
    std::ofstream branchFile(".minigit/branches/" + branchName);
    if (!branchFile) {
        std::cerr << "Failed to create branch\n";
        return;
    }
    branchFile << currentCommit;
    branchFile.close();
    std::cout << "Branch '" << branchName << "' created.\n";
}
