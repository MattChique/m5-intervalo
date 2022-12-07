#include "TimeLapse.h"
#include "Settings.h"
#include "CanonBLERemote.h"


bool TimeLapse::Initialize(Settings settings)
{
    _settings = settings;

    frameCounter = 0;
    timer = -1;

    return true;
}

TimeLapse::State TimeLapse::Process(int sampleRate)
{
    timer++;

    if(frameCounter >= _settings.framesNum)
    {
        //Serial.println("Finished");
        return Finished;
    }

    //Serial.println("- second " + String(timer) + " ---------------");

    if(_settings.delayBefore > 0 && timer < _settings.delayBefore)
    {
        //Serial.println("Delay before: " + String(_settings.delayBefore));              

        return Waiting;
    } 

    int current = timer - _settings.delayBefore;    

    //start exposing
    if( current == (frameCounter * _settings.lightTime) + (frameCounter * _settings.delayBetween) )
    {                
        //Serial.println("Trigger bulb #1 for frame " + String(frameCounter+1) );
        return Exposing;
    }                

    if( current == (frameCounter * _settings.lightTime) + (frameCounter * _settings.delayBetween) + _settings.lightTime )
    {
        //Serial.println("Trigger bulb #2 for frame " + String(frameCounter+1) );
        frameCounter++;
        return Exposing;
    }
        
    if( current <= (frameCounter * _settings.lightTime) + (frameCounter * _settings.delayBetween) )
    {                
        //Serial.println("delay between");
        return Waiting;
    }

    return None;
}

String TimeLapse::GetTotalTime()
{
    float timeFull = (_settings.framesNum * _settings.lightTime) + ((_settings.framesNum - 1) * _settings.delayBetween) + _settings.delayBefore;

    return String(timeFull);
}

String TimeLapse::GetTime()
{
    float timeFull = (_settings.framesNum * _settings.lightTime) + ((_settings.framesNum - 1) * _settings.delayBetween) + _settings.delayBefore;

    return String(timeFull - timer);
}