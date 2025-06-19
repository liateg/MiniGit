
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include "checkout.hpp"
namespace fs = std::filesystem;
void checkout(const std::string& name) {
    std::string commitHash;
    std::ifstream branchFile(".minigit/branches/" + name);
    if (branchFile) {
        std::getline(branchFile, commitHash);  
        branchFile.close();
        std::ofstream headFile(".minigit/HEAD");
        headFile << name;  
        headFile.close();
    } else {
        commitHash = name;
        std::ofstream headFile(".minigit/HEAD");
        headFile << commitHash;  
        headFile.close();
    }
    fs::path commitPath = ".minigit/commits/" + commitHash;
    if (!fs::exists(commitPath)) {
        std::cerr << "Commit or branch not found.\n";
        return;
    }
    for (const auto& file : fs::directory_iterator(commitPath)) {
        fs::copy_file(file, file.path().filename(), fs::copy_options::overwrite_existing);
    }
    std::cout << "Checked out " << name << "\n";
}
