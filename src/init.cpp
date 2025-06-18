#include <windows.h> //for setting the minigit hidden
#include <iostream>
#include <fstream>         // For file creation
#include <filesystem>      // For directory creation (C++17 and above)

namespace fs = std::filesystem;
using namespace std;

void initMiniGit() {

    //Create the .minigit
    string minigit = ".minigit";  //need to make it an obj to use string methods like c_str

    if (!fs::exists(minigit)) {
        fs::create_directory(minigit);
        SetFileAttributesA(minigit.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

        // Create the required internal folders
        fs::create_directories(minigit + "/objects");
        fs::create_directories(minigit + "/refs/heads");

        // Create HEAD file and point to main branch
        ofstream headFile(minigit + "/HEAD");
        headFile << "ref: refs/heads/main" << endl;
        headFile.close();

        // Create index file (empty)
        ofstream indexFile(minigit + "/index");
        indexFile.close();

        // Create the main branch file (empty for now)
        ofstream mainBranch(minigit + "/refs/heads/main");
        mainBranch.close();

        cout << "MiniGit initialized successfully";
    } else {
        cout << "MiniGit has already been initialized";
        return;
    }
}

