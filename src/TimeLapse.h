#ifndef TimeLapse_H_
#define TimeLapse_H_

#include <Arduino.h>
#include "Settings.h"
#include "CanonBLERemote.h"

class TimeLapse {
    public:
        enum State { None, Waiting, Exposing, Finished };
        bool Initialize(Settings settings);
        State Process(int sampleRate);
        String GetTotalTime();
        String GetTime();

    private:
        Settings _settings;
        int timer = 0;
        int frameCounter = 0;
};

#endif
