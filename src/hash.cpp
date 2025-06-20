#include "hash.hpp"
#include <openssl/sha.h>   // For SHA1 hashing
#include <iomanip>         // For hex formatting
#include <sstream>         // For string building


std::string computeHash(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH]; // Buffer for raw hash (20 bytes)
    
    // Generate SHA1 hash - converts input to binary hash
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    // Convert binary hash to readable hex string
    std::ostringstream oss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return oss.str(); // Returns 40-char hex like "a94a8fe5cc..."
}