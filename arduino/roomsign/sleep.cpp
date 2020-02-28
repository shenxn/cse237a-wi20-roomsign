#include <LowPower.h>


void sleep() {
    LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);
}
