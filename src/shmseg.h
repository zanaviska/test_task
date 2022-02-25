#pragma once

#include <atomic>

#define BUF_SIZE 1024
#define SHM_KEY_1 0x1234
#define SHM_KEY_2 0x4321

struct shmseg
{
    static_assert(std::atomic_bool::is_always_lock_free,
                  "atomic_bool not lock free, can't work in shared mem");
    std::atomic_bool filled;
    std::atomic_flag lock;
    char buf[BUF_SIZE];
};