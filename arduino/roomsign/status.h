#ifndef STATUS_H
#define STATUS_H

struct Status {
    bool available;
    bool updated;

    // access control
    bool authorized;
    bool unlocked;
    unsigned int unlockTime;
};

extern Status status;

#endif
