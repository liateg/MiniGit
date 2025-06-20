#ifndef SHA1_HPP
#define SHA1_HPP

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Lightweight, header-only SHA-1 implementation

class SHA1 {
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};

static const size_t BLOCK_INTS = 16;
static const size_t BLOCK_BYTES = BLOCK_INTS * 4;

inline static void reset(uint32_t digest[], std::string &buffer, uint64_t &transforms) {
    digest[0] = 0x67452301;
    digest[1] = 0xefcdab89;
    digest[2] = 0x98badcfe;
    digest[3] = 0x10325476;
    digest[4] = 0xc3d2e1f0;

    buffer = "";
    transforms = 0;
}

inline static uint32_t rol(const uint32_t value, const size_t bits) {
    return (value << bits) | (value >> (32 - bits));
}

inline static uint32_t blk(const uint32_t block[BLOCK_INTS], const size_t i) {
    return rol(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i], 1);
}

inline static void R0(const uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w,
                      const uint32_t x, const uint32_t y, uint32_t &z, const size_t i) {
    z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
    w = rol(w, 30);
}

inline static void R1(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w,
                      const uint32_t x, const uint32_t y, uint32_t &z, const size_t i) {
    block[i] = blk(block, i);
    z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
    w = rol(w, 30);
}

inline static void R2(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w,
                      const uint32_t x, const uint32_t y, uint32_t &z, const size_t i) {
    block[i] = blk(block, i);
    z += (w ^ x ^ y) + block[i] + 0x6ed9eba1 + rol(v, 5);
    w = rol(w, 30);
}

inline static void R3(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w,
                      const uint32_t x, const uint32_t y, uint32_t &z, const size_t i) {
    block[i] = blk(block, i);
    z += (((w | x) & y) | (w & x)) + block[i] + 0x8f1bbcdc + rol(v, 5);
    w = rol(w, 30);
}

inline static void R4(uint32_t block[BLOCK_INTS], const uint32_t v, uint32_t &w,
                      const uint32_t x, const uint32_t y, uint32_t &z, const size_t i) {
    block[i] = blk(block, i);
    z += (w ^ x ^ y) + block[i] + 0xca62c1d6 + rol(v, 5);
    w = rol(w, 30);
}

inline static void transform(uint32_t digest[], uint32_t block[BLOCK_INTS], uint64_t &transforms) {
    uint32_t a = digest[0], b = digest[1], c = digest[2], d = digest[3], e = digest[4];

    R0(block, a, b, c, d, e,  0); R0(block, e, a, b, c, d,  1); R0(block, d, e, a, b, c,  2);
    R0(block, c, d, e, a, b,  3); R0(block, b, c, d, e, a,  4); R0(block, a, b, c, d, e,  5);
    R0(block, e, a, b, c, d,  6); R0(block, d, e, a, b, c,  7); R0(block, c, d, e, a, b,  8);
    R0(block, b, c, d, e, a,  9); R0(block, a, b, c, d, e, 10); R0(block, e, a, b, c, d, 11);
    R0(block, d, e, a, b, c, 12); R0(block, c, d, e, a, b, 13); R0(block, b, c, d, e, a, 14);
    R0(block, a, b, c, d, e, 15);

    R1(block, e, a, b, c, d,  0); R1(block, d, e, a, b, c,  1); R1(block, c, d, e, a, b,  2);
    R1(block, b, c, d, e, a,  3); R2(block, a, b, c, d, e,  4); R2(block, e, a, b, c, d,  5);
    R2(block, d, e, a, b, c,  6); R2(block, c, d, e, a, b,  7); R2(block, b, c, d, e, a,  8);
    R2(block, a, b, c, d, e,  9); R2(block, e, a, b, c, d, 10); R2(block, d, e, a, b, c, 11);
    R2(block, c, d, e, a, b, 12); R2(block, b, c, d, e, a, 13); R2(block, a, b, c, d, e, 14);
    R2(block, e, a, b, c, d, 15); R2(block, d, e, a, b, c,  0); R2(block, c, d, e, a, b,  1);
    R2(block, b, c, d, e, a,  2); R2(block, a, b, c, d, e,  3); R2(block, e, a, b, c, d,  4);
    R2(block, d, e, a, b, c,  5); R2(block, c, d, e, a, b,  6); R2(block, b, c, d, e, a,  7);
    R3(block, a, b, c, d, e,  8); R3(block, e, a, b, c, d,  9); R3(block, d, e, a, b, c, 10);
    R3(block, c, d, e, a, b, 11); R3(block, b, c, d, e, a, 12); R3(block, a, b, c, d, e, 13);
    R3(block, e, a, b, c, d, 14); R3(block, d, e, a, b, c, 15); R3(block, c, d, e, a, b,  0);
    R3(block, b, c, d, e, a,  1); R3(block, a, b, c, d, e,  2); R3(block, e, a, b, c, d,  3);
    R3(block, d, e, a, b, c,  4); R3(block, c, d, e, a, b,  5); R3(block, b, c, d, e, a,  6);
    R3(block, a, b, c, d, e,  7); R3(block, e, a, b, c, d,  8); R3(block, d, e, a, b, c,  9);
    R3(block, c, d, e, a, b, 10); R3(block, b, c, d, e, a, 11); R4(block, a, b, c, d, e, 12);
    R4(block, e, a, b, c, d, 13); R4(block, d, e, a, b, c, 14); R4(block, c, d, e, a, b, 15);
    R4(block, b, c, d, e, a,  0); R4(block, a, b, c, d, e,  1); R4(block, e, a, b, c, d,  2);
    R4(block, d, e, a, b, c,  3); R4(block, c, d, e, a, b,  4); R4(block, b, c, d, e, a,  5);
    R4(block, a, b, c, d, e,  6); R4(block, e, a, b, c, d,  7); R4(block, d, e, a, b, c,  8);
    R4(block, c, d, e, a, b,  9); R4(block, b, c, d, e, a, 10); R4(block, a, b, c, d, e, 11);
    R4(block, e, a, b, c, d, 12); R4(block, d, e, a, b, c, 13); R4(block, c, d, e, a, b, 14);
    R4(block, b, c, d, e, a, 15);

    digest[0] += a; digest[1] += b; digest[2] += c;
    digest[3] += d; digest[4] += e;
    transforms++;
}

inline static void buffer_to_block(const std::string &buffer, uint32_t block[BLOCK_INTS]) {
    for (size_t i = 0; i < BLOCK_INTS; i++) {
        block[i] = (buffer[4*i+3] & 0xff)
                 | (buffer[4*i+2] & 0xff) << 8
                 | (buffer[4*i+1] & 0xff) << 16
                 | (buffer[4*i+0] & 0xff) << 24;
    }
}

inline SHA1::SHA1() {
    reset(digest, buffer, transforms);
}

inline void SHA1::update(const std::string &s) {
    std::istringstream is(s);
    update(is);
}

inline void SHA1::update(std::istream &is) {
    while (true) {
        char sbuf[BLOCK_BYTES];
        is.read(sbuf, BLOCK_BYTES - buffer.size());
        buffer.append(sbuf, static_cast<size_t>(is.gcount()));
        if (buffer.size() != BLOCK_BYTES) return;

        uint32_t block[BLOCK_INTS];
        buffer_to_block(buffer, block);
        transform(digest, block, transforms);
        buffer.clear();
    }
}

inline std::string SHA1::final() {
    uint64_t total_bits = (transforms * BLOCK_BYTES + buffer.size()) * 8;
    buffer += (char)0x80;
    size_t orig_size = buffer.size();
    while (buffer.size() < BLOCK_BYTES) buffer += (char)0x00;

    uint32_t block[BLOCK_INTS];
    buffer_to_block(buffer, block);

    if (orig_size > BLOCK_BYTES - 8) {
        transform(digest, block, transforms);
        std::fill(block, block + BLOCK_INTS - 2, 0);
    }

    block[BLOCK_INTS - 1] = (uint32_t)total_bits;
    block[BLOCK_INTS - 2] = (uint32_t)(total_bits >> 32);
    transform(digest, block, transforms);

    std::ostringstream result;
    for (size_t i = 0; i < sizeof(digest) / sizeof(digest[0]); i++)
        result << std::hex << std::setfill('0') << std::setw(8) << digest[i];

    reset(digest, buffer, transforms);
    return result.str();
}

inline std::string SHA1::from_file(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    SHA1 checksum;
    checksum.update(stream);
    return checksum.final();
}

#endif // SHA1_HPP
