#ifndef MACRO_H
#define MACRO_H

// #define DEBUG
#ifdef DEBUG
    #define SERIAL_PRINT(a) Serial.print(a);
    #define SERIAL_PRINTLN(a) Serial.println(a);
#else
    #define SERIAL_PRINT(a) ;
    #define SERIAL_PRINTLN(a) ;
#endif

#endif
