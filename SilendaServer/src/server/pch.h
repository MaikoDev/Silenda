#pragma once

#include <iostream>

#include <sstream>
#include <string>

#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>

#include <random>

#include <chrono>
#include <ctime>

#include <thread>
#include <mutex>

#define NOMINMAX
#include <Windows.h>

#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")

#include "msgpack.hpp"

#include "cryptopp/filters.h"
#include "cryptopp/osrng.h"
#include "cryptopp/gzip.h"
#include "cryptopp/rsa.h"
#include "cryptopp/hex.h"

#include "nlohmann/json.hpp"