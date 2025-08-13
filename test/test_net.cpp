#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <string>
#include "../net.h"

TEST(ParseAddressTest, ValidAddress) {
    sockaddr_in addr = parseAddress("127.0.0.1:8080");

    EXPECT_EQ(addr.sin_family, AF_INET);
    EXPECT_EQ(ntohs(addr.sin_port), 8080);

    char ipBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipBuffer, sizeof(ipBuffer));
    EXPECT_EQ(std::string(ipBuffer), "127.0.0.1");
}

TEST(ParseAddressTest, ThrowsWhenMissingColon) {
    EXPECT_THROW(parseAddress("127.0.0.1"), std::invalid_argument);
}

TEST(ParseAddressTest, ThrowsWhenInvalidPortNonNumeric) {
    EXPECT_THROW(parseAddress("127.0.0.1:abc"), std::invalid_argument);
}

TEST(ParseAddressTest, ThrowsWhenPortOutOfRangeLow) {
    EXPECT_THROW(parseAddress("127.0.0.1:-1"), std::out_of_range);
}

TEST(ParseAddressTest, ThrowsWhenPortOutOfRangeHigh) {
    EXPECT_THROW(parseAddress("127.0.0.1:70000"), std::out_of_range);
}

TEST(ParseAddressTest, ThrowsWhenInvalidIp) {
    EXPECT_THROW(parseAddress("999.999.999.999:8080"), std::invalid_argument);
}
