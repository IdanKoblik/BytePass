#include <gtest/gtest.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <thread>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "../server.h"
#include "../driver.h"

namespace fs = std::filesystem;

const unsigned int TEST_PORT = 55555;
const std::string OUTPUT_DIR = "/tmp";
const std::string TEST_FILE = "/tmp/test_input.txt";
const std::string RECON_FILE = OUTPUT_DIR + "/test_input.txt";

void writeTestFile() {
    std::ofstream ofs(TEST_FILE, std::ios::binary);
    ofs << "HelloBytepass!";
    ofs.close();
}

void writeBigTestFile() {
    std::ofstream ofs(TEST_FILE, std::ios::binary);
    std::string chunk(1500, 'A'); 
    ofs << chunk;
    ofs.close();
}

TEST(ServerErrorTest, InvalidPort) {
    int ret = runServer(70000, "/tmp");
    EXPECT_EQ(ret, -1);
}

TEST(DriverErrorTest, FileNotFound) {
    int ret = runDriver("0.0.0.0:55555", "/tmp/nonexistent.txt");
    EXPECT_EQ(ret, -1);
}

TEST(ServerErrorTest, BindFailure) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_GE(sockfd, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(TEST_PORT);

    ASSERT_EQ(bind(sockfd, (sockaddr*)&addr, sizeof(addr)), 0);

    int ret = runServer(TEST_PORT, OUTPUT_DIR);
    EXPECT_EQ(ret, -1);

    close(sockfd);
}


TEST(BytepassE2E, RunServerAndDriver) {
    writeTestFile();

    std::thread serverThread([&]() {
        int ret = runServer(TEST_PORT, OUTPUT_DIR);
        EXPECT_EQ(ret, 0);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int driverRet = runDriver("0.0.0.0:" + std::to_string(TEST_PORT), TEST_FILE);
    EXPECT_EQ(driverRet, 0);

    serverThread.join();

    std::ifstream ifs(RECON_FILE, std::ios::binary);
    ASSERT_TRUE(ifs.is_open());

    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "HelloBytepass!");

    fs::remove(TEST_FILE);
    fs::remove(RECON_FILE);
}

TEST(BytepassE2E, MultiChunkFile) {
    writeBigTestFile();

    std::thread serverThread([&]() {
        int ret = runServer(TEST_PORT, OUTPUT_DIR);
        EXPECT_EQ(ret, 0);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int driverRet = runDriver("127.0.0.1:" + std::to_string(TEST_PORT), TEST_FILE);
    EXPECT_EQ(driverRet, 0);

    serverThread.join();

    std::ifstream ifs(RECON_FILE, std::ios::binary);
    ASSERT_TRUE(ifs.is_open());

    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content.size(), 1500);
    EXPECT_TRUE(std::all_of(content.begin(), content.end(),
                            [](char c){ return c == 'A'; }));

    fs::remove(TEST_FILE);
    fs::remove(RECON_FILE);
}

