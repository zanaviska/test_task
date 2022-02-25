#pragma once

#include "shmseg.h"

class sm_receiver
{
    bool is_open = false;
    int shmid;
    shmseg *shmp;

public:
    sm_receiver(int key) { open(key); }
    void open(int key);
    std::string receive();
    void close();
    ~sm_receiver();
};