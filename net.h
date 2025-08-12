#ifndef NET_H
#define NET_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <iostream>

#define MIN_PORT 0
#define MAX_PORT 65535

sockaddr_in parseAddress(const std::string &addrStr);

#endif
