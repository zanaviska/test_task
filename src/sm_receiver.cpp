#include "sm_receiver.h"

#include <iostream>
#include <thread>

#include <sys/shm.h>
#include <sys/types.h>

#include "lock_guard.h"

void sm_receiver::open(int key)
{
    shmid = shmget(key, sizeof(shmseg), 0644 | IPC_CREAT);
    if (shmid == -1)
    {
        is_open = false;
        return;
    }

    // Attach to the segment to get a pointer to it.
    shmp = reinterpret_cast<shmseg *>(shmat(shmid, NULL, 0));
    if (shmp == (void *)-1)
    {
        is_open = false;
        return;
    }
}

std::string sm_receiver::receive()
{
    std::string res;
    res.reserve(BUF_SIZE);

    while (true)
    {
        std::this_thread::yield();

        lock_guard lg(shmp->lock);
        if (shmp->filled.load())
        {
            for (int i = 0; shmp->buf[i] != 0; i++)
                res.push_back(shmp->buf[i]);

            shmp->filled = false;
            break;
        }
    }
    return res;
}
void sm_receiver::close()
{
    shmdt(shmp);
}

sm_receiver::~sm_receiver()
{
    shmp->filled = false;
    shmp->lock.clear();
    close();
}