#ifndef LOG_HPP
#define LOG_HPP

#include <string>

struct CommitNode {
    std::string hash;
    std::string parentHash;
    std::string message;
    std::string timestamp;
    CommitNode* next;
};

CommitNode* createCommitList();
void printCommitLog(CommitNode* head);
void freeCommitList(CommitNode* head);
void handleLog();

#endif // LOG_HPP
