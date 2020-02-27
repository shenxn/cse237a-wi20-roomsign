#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>

struct Status {
    uint8_t key_id[4];

    // access control
    bool authorized;
    bool unlocked;
    unsigned long unlock_time;
};

extern Status status;

#endif
