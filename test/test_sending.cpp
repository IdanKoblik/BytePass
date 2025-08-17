#include <gtest/gtest.h>
#include <thread>
#include <fstream>
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

TEST(ServerErrorTest, InvalidPort) {
    int ret = runServer(70000, "/tmp");
    EXPECT_EQ(ret, -1);
}

TEST(DriverErrorTest, FileNotFound) {
    int ret = runDriver("0.0.0.0:55555", "/tmp/nonexistent.txt");
    EXPECT_EQ(ret, -1);
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

