#include <exception>

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "init.hpp"
#include "add.hpp"
#include "diff.hpp"
#include "branch.hpp" 


#include "commit.hpp"  
#include "log.hpp"
#include "checkout.hpp"
#include "merge.hpp"

using namespace std;


void printUsage() {
    cout << "Usage: minigit <command> [options]" << endl;
    cout << "Available commands:" << endl;
    cout << "  init                  Initialize repository" << endl;
    cout << "  add <filename>        Add file to staging area" << endl;
    cout << "  commit -m <message>   Commit staged changes" << endl;
    cout << "  log                   Show commit history" << endl;
    cout << "  branch <name>         Create a new branch" << endl;
    cout << "  checkout <target>     Switch to branch or commit" << endl;
    cout << "  merge <branch>        Merge another branch into current" << endl;
    cout << "  diff <c1> <c2>        Compare changes between commits" << endl;
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
        createcommit(message, indexMap);
    }else if (command == "log") {
 
   handleLog();
    


}else if (command == "branch") {
    if (argc < 3) {
        std::cerr << "Usage: branch <branch-name>\n";
        return 1;
    }
    createBranch(argv[2]);
}
else if (command == "checkout") {
    if (argc < 3) {
        cerr << "Usage: checkout <branch-name|commit-hash>\n";
        return 1;
    }
    string target = argv[2];
    checkout(target);
}else if (command == "merge") {
    if (argc < 3) {
        cerr << "Usage: merge <branch-name>\n";
        return 1;
    }
    mergeBranch(argv[2]);
}
else if (command == "diff") {
    if (argc < 4) {
        std::cerr << "Usage: diff <commit1> <commit2>\n";
        return 1;
    }
    diffCommits(argv[2], argv[3]);
}


else {
        cerr << "Unknown command: " << command << endl;
        printUsage();
        return 1;
    }

    return 0;
}
