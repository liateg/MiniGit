#include "log.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
using namespace std;

void handleLog() {
    string branch = getCurrentBranch();
    cout << "Current branch: " << branch << endl;

    // Read HEAD commit hash from refs
    ifstream headFile(".minigit/refs/heads/" + branch);
    string currentHash;
    if (!getline(headFile, currentHash) || currentHash.empty() || currentHash == string(40, '0')) {
        cout << "No commits found.\n";
        return;
    }

    while (!currentHash.empty() && currentHash != string(40, '0')) {
        fs::path commitPath = ".minigit/objects/" + currentHash;
        ifstream commitFile(commitPath);
        if (!commitFile) {
            cerr << "Error reading commit file: " << currentHash << endl;
            break;
        }

        string line, message, timestamp, parentHash;

        while (getline(commitFile, line)) {
            if (line.rfind("message: ", 0) == 0) {
                message = line.substr(9);
            } else if (line.rfind("timestamp: ", 0) == 0) {
                timestamp = line.substr(11);
            } else if (line.rfind("parent: ", 0) == 0) {
                parentHash = line.substr(8);
            }
        }

        cout << "Commit: " << currentHash << endl;
        cout << "Date: " << timestamp << endl;
        cout << "Message: " << message << endl;
        cout << "--------------------------\n";

        currentHash = parentHash;
    }
}
