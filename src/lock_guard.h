#pragma once

#include <atomic>

class lock_guard
{
    std::atomic_flag &_flag;

public:
    lock_guard(std::atomic_flag &flag);
    ~lock_guard();
};