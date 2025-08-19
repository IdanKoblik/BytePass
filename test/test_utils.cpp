#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <cstdio>
#include "../utils.h"

TEST(ParseAddressTest, ValidAddress) {
    sockaddr_in addr = parseAddress("127.0.0.1:8080");

    EXPECT_EQ(addr.sin_family, AF_INET);
    EXPECT_EQ(ntohs(addr.sin_port), 8080);

    char ipBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipBuffer, sizeof(ipBuffer));
    EXPECT_EQ(std::string(ipBuffer), "127.0.0.1");
} 

TEST(ParseAddressTest, InvalidAddress) {
    EXPECT_THROW(parseAddress("127.0.0.1"), std::invalid_argument);
    EXPECT_THROW(parseAddress("127.0.0.1:abc"), std::invalid_argument);
    EXPECT_THROW(parseAddress("127.0.0.1:-1"), std::out_of_range); 
    EXPECT_THROW(parseAddress("127.0.0.1:70000"), std::out_of_range);
    EXPECT_THROW(parseAddress("999.999.999.999:8080"), std::invalid_argument);
}

TEST(TrimTest, GeneralCases) {
    struct {
        std::string input;
        std::string expected;
    } cases[] = {
        {"   hello   ", "hello"},
        {"\n\t  world \r\n", "world"},
        {"", ""},
        {"   \t\n  ", ""},
        {"test", "test"}
    };

    for (auto &c : cases) {
        EXPECT_EQ(trim(c.input), c.expected);
    }
}

TEST(SanitizeFilenameTest, GeneralCases) {
    struct {
        std::string input;
        std::string expected;
    } cases[] = {
        {"hello_world-123.txt", "hello_world-123.txt"}, // allowed chars
        {"he!!o@wor?ld.txt", "heoworld.txt"},           // remove unsafe
        {"/etc/passwd", "passwd"},                      // strip unix path
        {"C:\\Windows\\system32\\cmd.exe", "cmd.exe"},  // strip windows path
        {"../secret.txt", "secret.txt"},                // remove ..
        {"....hidden", "hidden"},                       // collapse ..
        {"", "file"},                                   // empty
        {"////", "file"}                                // path only
    };

    for (auto &c : cases) {
        EXPECT_EQ(sanitizeFilename(c.input), c.expected);
    }
}

TEST(ChecksumTest, HelloWorldFile) {
    const char* filename = "hello.txt";
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "hello";
    ofs.close();

    std::string expected = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
    EXPECT_EQ(calcChecksum(filename), expected);

    std::remove(filename); 
}

TEST(ChecksumTest, EmptyFile) {
    const char* filename = "empty.txt";
    std::ofstream ofs(filename, std::ios::binary); 
    ofs.close();

    std::string expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    EXPECT_EQ(calcChecksum(filename), expected);

    std::remove(filename);
}

TEST(ChecksumTest, DifferentFiles) {
    const char* file1 = "file1.txt";
    const char* file2 = "file2.txt";
    std::ofstream(file1) << "hello";
    std::ofstream(file2) << "world";

    EXPECT_NE(calcChecksum(file1), calcChecksum(file2));

    std::remove(file1);
    std::remove(file2);
}

TEST(ChecksumTest, DeterministicOutput) {
    const char* filename = "gtest.txt";
    std::ofstream(filename) << "gtest";
    std::ofstream ofs(filename, std::ios::app); 

    auto hash1 = calcChecksum(filename);
    auto hash2 = calcChecksum(filename);

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1.size(), 64);

    std::remove(filename);
}

TEST(ChecksumTest, NonExistingFile) {
   EXPECT_THROW(calcChecksum("notfound.txt"), std::runtime_error);
}
