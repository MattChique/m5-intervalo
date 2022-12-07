#ifndef Settings_H_
#define Settings_H_

#include <Arduino.h>

class Settings {
    public :
        enum Connection { Bluetooth, Direct };

        int framesNum = 5;
        int delayBefore = 1;
        int delayBetween = 3;
        float lightTime = 3.0;
        Connection connectionMode = Bluetooth;
};

#endif
