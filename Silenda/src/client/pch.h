#pragma once

#include <iostream>
#include <stdio.h>

#include <sstream>
#include <string>

#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>

#include <memory>

#include <chrono>
#include <ctime>

#include <thread>
#include <mutex>

#define NOMINMAX
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "msgpack.hpp"

#include "cryptopp/filters.h"
#include "cryptopp/osrng.h"
#include "cryptopp/gzip.h"
#include "cryptopp/rsa.h"
#include "cryptopp/hex.h"

#include "nlohmann/json.hpp"