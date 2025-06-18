#include <iostream>
#include <string>
#include "init.hpp"  

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Welcome to MiniGit!" << endl;

    if (argc < 2) {
        cout << "Usage: minigit <command>" << endl;
        cout << "Available commands: init" << endl;
        return 1;
    }

    string command = argv[1];

    if (command == "init") {
       initMiniGit(); 
    } else {
        cout << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}
