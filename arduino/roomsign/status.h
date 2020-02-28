#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>

struct Event {
    byte available;
    char summary[17];
    char time[21];
    char creator[18];
    byte key_id[4];
};

struct Status {
    bool updated;

    Event event;

    byte last_payload[32];

    unsigned long clock;

    // access control
    bool authorized;
    bool unlocked;
    unsigned long unlock_time;
};

extern Status status;

#endif
