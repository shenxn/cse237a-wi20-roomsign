#ifndef STATUS_H
#define STATUS_H

struct Event {
    char available;
    char summary[21];
    char time[21];
    char creator[21];
};

struct Status {
    bool updated;

    Event event;

    // access control
    bool authorized;
    bool unlocked;
    unsigned int unlockTime;
};

extern Status status;

#endif
