#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>

struct Event {
    byte available;
    char summary[23];
    char time[23];
    char creator[27];
    byte key_id[4];
};

struct Status {
    bool updated;

    Event event;

    unsigned long clock;

    // access control
    bool authorized;
    bool unlocked;
    unsigned long unlock_time;
};

extern Status status;

#endif
