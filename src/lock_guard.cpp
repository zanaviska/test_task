#include "lock_guard.h"

#include <thread>

lock_guard::lock_guard(std::atomic_flag &flag) : _flag{flag}
{
    while (_flag.test_and_set())
    {
        std::this_thread::yield();
    }
}

lock_guard::~lock_guard()
{
    _flag.clear();
}