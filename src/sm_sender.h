#pragma once

#include <string>

#include "shmseg.h"

class sm_sender
{
    shmseg *shmp;
    int shmid;
    bool is_open = false;

public:
    sm_sender(int key) { open(key); }
    void open(int key);
    bool send(const std::string &str);
    void close();
};