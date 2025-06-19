#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "init.hpp"
#include "add.hpp"
#include "commit.hpp"  // <-- NEW

using namespace std;

void printUsage() {
    cout << "Usage: minigit <command> [options]" << endl;
    cout << "Available commands:" << endl;
    cout << "  init                  Initialize repository" << endl;
    cout << "  add <filename>        Add file to staging area" << endl;
    cout << "  commit -m <message>   Create a new commit" << endl;  // <-- NEW
}

int main(int argc, char* argv[]) {
    cout << "Welcome to MiniGit!" << endl;

    if (argc < 2) {
        printUsage();
        return 1;
    }

    string command = argv[1];

    if (command == "init") {
        initMiniGit();
    }
    else if (command == "add") {
        if (argc < 3) {
            cerr << "Error: 'add' requires a filename.\n";
            return 1;
        }
        string filename = argv[2];
        addFileToStaging(filename);
    }
    else if (command == "commit") {
        if (argc < 4 || string(argv[2]) != "-m") {
            cerr << "Error: Usage is 'commit -m <message>'\n";
            return 1;
        }

        string message = argv[3];
        auto indexMap = parseIndex();
        createCommit(message, indexMap);
    }
    else {
        cerr << "Unknown command: " << command << endl;
        printUsage();
        return 1;
    }

    return 0;
}
