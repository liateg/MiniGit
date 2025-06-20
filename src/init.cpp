#include <exception>

#include <windows.h> 
#include <iostream>
#include <fstream>         
#include <filesystem>      

namespace fs = std::filesystem;
using namespace std;

void initMiniGit() {

    
    string minigit = ".minigit";  

    if (!fs::exists(minigit)) {
        fs::create_directory(minigit);
        SetFileAttributesA(minigit.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM); 

      
        fs::create_directories(minigit + "/objects");
        fs::create_directories(minigit + "/refs/heads");
        fs::create_directories(minigit + "/commits");

      
        ofstream headFile(minigit + "/HEAD");
        headFile << "ref: refs/heads/main" << endl; 
        headFile.close();

        
        ofstream indexFile(minigit + "/index");
        indexFile.close();

        ofstream mainBranch(minigit + "/refs/heads/main");
        mainBranch.close();

        cout << "MiniGit initialized successfully";
    } else {
        cout << "MiniGit has already been initialized";
        return;
    }
}

