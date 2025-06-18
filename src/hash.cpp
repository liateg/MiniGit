// hash.cpp
#include "hash.hpp"
#include "sha1.hpp"

std::string computeHash(const std::string& filePath) {
    return SHA1::from_file(filePath); //this is the standard hashing mentioned on the assginmnt 
}
