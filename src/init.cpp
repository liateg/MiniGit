#ifdef _WIN32
#include <windows.h>
#endif //for setting the minigit hidden
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
        #ifdef _WIN32
SetFileAttributes(".minigit", FILE_ATTRIBUTE_HIDDEN);
#endif //make it hidden and system

      
        fs::create_directories(minigit + "/objects");
        fs::create_directories(minigit + "/refs/heads");

      
        ofstream headFile(minigit + "/HEAD");
        headFile << "ref: refs/heads/main" << endl; //default branch is main
        headFile.close();

        
        ofstream indexFile(minigit + "/index");
        indexFile.close();

        // main branch file (empty for now)
        ofstream mainBranch(minigit + "/refs/heads/main");
        mainBranch.close();

        cout << "MiniGit initialized successfully";
    } else {
        cout << "MiniGit has already been initialized";
        return;
    }
}

