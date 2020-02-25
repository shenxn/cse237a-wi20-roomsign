#ifndef STATUS_H
#define STATUS_H

struct Status {
    bool available;
    bool updated;

    char name[16];
    char time[16];

    // access control
    bool authorized;
    bool unlocked;
    unsigned int unlockTime;
};

extern Status status;

#endif
