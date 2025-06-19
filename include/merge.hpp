#ifndef MERGE_HPP
#define MERGE_HPP

#include <string>

struct CommitNode; // forward declaration

CommitNode* findLowestCommonAncestor(CommitNode* a, CommitNode* b);
void mergeBranches(CommitNode* current, CommitNode* target);
void showDiff(const std::string& file1, const std::string& file2);

#endif
