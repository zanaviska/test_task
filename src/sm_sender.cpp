#include "sm_sender.h"

#include <iostream>

#include <sys/shm.h>
#include <sys/types.h>

#include "lock_guard.h"

void sm_sender::open(int key)
{
    shmid = shmget(key, sizeof(struct shmseg), 0644 | IPC_CREAT);
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
    is_open = true;
}

bool sm_sender::send(const std::string &str)
{
    if (str.length() > 1023) return false;

    lock_guard lg(shmp->lock);
    /* Transfer blocks of data from buffer to shared memory */
    for (int i = 0; i < str.length(); i++)
        shmp->buf[i] = str[i];
    shmp->buf[str.length()] = 0;
    shmp->filled = true;

    return true;
}

void sm_sender::close()
{
    shmdt(shmp);

    shmctl(shmid, IPC_RMID, 0);
}