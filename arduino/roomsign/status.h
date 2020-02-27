#ifndef STATUS_H
#define STATUS_H

struct Event {
    char available;
    char summary[17];
    char time[21];
    char creator[18];
    char key_id[4];
};

struct Status {
    bool updated;

    Event event;

    // access control
    bool authorized;
    bool unlocked;
    unsigned long unlock_time;
};

extern Status status;

#endif
