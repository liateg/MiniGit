#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <unordered_map>

using namespace std;

string getCurrentBranch();
string getParentHash(const string& cbranch);
std::unordered_map<std::string, std::string> getCommitSnapshot(const std::string& commitHash);


#endif
