#pragma once

#include <filesystem>

using namespace std;

struct Folder {
    filesystem::path path;
    uint64_t size = 0;
};