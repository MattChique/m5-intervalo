#include "CanonBLERemote.h"
#include <Arduino.h>
#include "Ticker.h"
#include "M5StickCPlus.h"
#include "Display.h"
#include "TimeLapse.h"
#include "Settings.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include <esp32-hal-log.h>

String name = "M5 Intervalo";
CanonBLERemote canon_ble(name);
Display M5_display(&M5.Lcd, name);

TimeLapse timelapse;
Settings settings;
bool expose = false;

enum RemoteMode {Setup, Shooting} currentMode;
enum ScreenType {Main, Menu, Intervall, Delay, Frames, Exposure, Connection} currentScreen;

hw_timer_t * timer = NULL; //https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {

    portENTER_CRITICAL_ISR(&timerMux);

    if(currentMode == Shooting)
    {
        TimeLapse::State state = timelapse.Process(1);

        if(state == timelapse.Finished)
            currentMode = Setup;

        if(state == timelapse.Exposing)
            expose = true;
    }

    portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
    Serial.begin(115200);
    esp_log_level_set("*", ESP_LOG_INFO); 

    M5.begin();
    
    M5.Axp.ScreenBreath(9);
    M5.Lcd.setRotation(1);
    
    M5_display.set_init_screen();

    currentMode = Setup;

    canon_ble.init();
    
    delay(500);

    // Pairing
    if(settings.connectionMode == settings.Bluetooth && 1 == 0)
    {       
        M5_display.printBottom("Pairing...");
        do{
            Serial.println("Pairing...");
        }
        while(!canon_ble.pair(10));
    }

    delay(500);
    Serial.println("Setup Done");

    //Timer
    timer = timerBegin(0,80,true); //(Use timer 0,80Mhz crystal thus set divider of 80 -> (1/(80MHz/80)))
    timerAttachInterrupt(timer,&onTimer,true); //Attach timer function to timer
    timerAlarmWrite(timer,1000000,true); //Set alarm to call timer fucntion every 1 second since 1 tick is 1 microsecond, third parameter is for it to repeat it
    timerAlarmEnable(timer); //enable timer
    Serial.println("Start Timer");

    timelapse.Initialize(settings);
}

unsigned long previousMillis = 0; 
unsigned long startmillis = 0;
int shootingDots = 3;

void loop()
{
    unsigned long currentMillis = millis();

    if(expose)
    {
        if(settings.connectionMode == Settings::Connection::Bluetooth)
        {
            Serial.println("Trigger bluetooth" + String(currentMillis));
            
            if(!canon_ble.trigger()){
                Serial.println("Trigger Failed");
            }
        }
        else
        {
            Serial.println("Trigger cable");
        }
        expose = false;
    }    

    if (currentMillis - previousMillis >= 200) 
    {
        M5.update();

        previousMillis = currentMillis;

        ScreenType nextScreen = currentScreen;

        M5_display.set_main_screen();

/* -------------------------------------------------------------------------------------------------------
    Handle buttons
-------------------------------------------------------------------------------------------------------*/
        if(currentScreen == Main)
        {
            if (M5.BtnB.wasReleased() && currentMode == Setup)
            {
                nextScreen = Intervall;
            }
            if (M5.BtnA.wasReleased() && currentMode == Setup)
            {
                timelapse.Initialize(settings);

                currentMode = Shooting;
            }
            if (M5.BtnA.wasReleasefor(700) && currentMode == Shooting)
            {
                currentMode = Setup;
            }
        }
        if(currentScreen == Intervall)
        {
            if (M5.BtnB.wasReleased())
            {
                nextScreen = Frames;
            }
            if (M5.BtnB.wasReleasefor(700))
            {
                nextScreen = Main;
            }
            if (M5.BtnA.wasReleased())
            {
                settings.delayBetween++;
            }
            if (M5.BtnA.wasReleasefor(700))
            {
                settings.delayBetween--;
            }
        }
        if(currentScreen == Frames)
        {
            if (M5.BtnB.wasReleased())
            {
                nextScreen = Delay;
            }
            if (M5.BtnB.wasReleasefor(700))
            {
                nextScreen = Main;
            }
            if (M5.BtnA.wasReleased())
            {
                settings.framesNum++;
            }
            if (M5.BtnA.wasReleasefor(700))
            {
                settings.framesNum--;
            }
        }
        if(currentScreen == Delay)
        {
            if (M5.BtnB.wasReleased())
            {
                nextScreen = Exposure;
            }
            if (M5.BtnB.wasReleasefor(700))
            {
                nextScreen = Main;
            }
            if (M5.BtnA.wasReleased())
            {
                settings.delayBefore++;
            }
            if (M5.BtnA.wasReleasefor(700))
            {
                settings.delayBefore--;
            }
        }
        if(currentScreen == Exposure)
        {
            if (M5.BtnB.wasReleased())
            {
                nextScreen = Connection;
            }
            if (M5.BtnB.wasReleasefor(700))
            {
                nextScreen = Main;
            }
            if (M5.BtnA.wasReleased())
            {
                settings.lightTime++;
            }
            if (M5.BtnA.wasReleasefor(700))
            {
                settings.lightTime--;
            }
        }
        if(currentScreen == Connection)
        {
            if (M5.BtnB.wasReleased())
            {
                nextScreen = Intervall;
            }
            if (M5.BtnB.wasReleasefor(700))
            {
                nextScreen = Main;
            }
            if (M5.BtnA.wasReleased())
            {
                if(settings.connectionMode == settings.Bluetooth)
                {
                    settings.connectionMode = settings.Direct;
                }
                else
                {
                    settings.connectionMode = settings.Bluetooth;
                }
            }
            if (M5.BtnA.wasReleasefor(700))
            {
                if(settings.connectionMode == settings.Bluetooth)
                {
                    int max = 5;
                    do{
                        Serial.println("Pairing...");
                        M5_display.printBottom("Pairing...");
                        max--;
                    }
                    while(!canon_ble.pair(10) && max > 0);
                }
            }
        }
/* -------------------------------------------------------------------------------------------------------
    Update display
-------------------------------------------------------------------------------------------------------*/
        double vaps = M5.Axp.GetVapsData() * 1.4 / 1000;  
        int batteryLevel = 100.0 * ((vaps - 3.0) / (4.07 - 3.0));
        M5_display.printBattery(String(batteryLevel > 100 ? 100 : batteryLevel) + "%");

        if(currentScreen == Main)
        {
            if(currentMode == Shooting)
            {
                String dots = "";

                if (currentMillis - previousMillis >= 200) {
                    
                    if (shootingDots == 3) 
                    {
                        shootingDots = 0;
                    }
                    else
                    {
                        shootingDots++;
                    }
                    for(int i=0; i < shootingDots; i++)
                    {
                        dots = dots + ".";
                    }
                }

                M5_display.printTop("Shooting" + dots);
                M5_display.printBottom("Time: " + String(timelapse.GetTime()));
            }
            else
            {
                M5_display.printTop("Overview");
                M5_display.printBottom("Total: " + String(timelapse.GetTotalTime()) + "s");
            }
        }
        if(currentScreen == Intervall)
        {
            M5_display.printTop("Time between Frames");        
            M5_display.printBottom(String(settings.delayBetween) + "s");
        }
        if(currentScreen == Frames)
        {
            M5_display.printTop("Number of frames");        
            M5_display.printBottom(String(settings.framesNum) + " x");
        }
        if(currentScreen == Delay)
        {
            M5_display.printTop("Delay before start");        
            M5_display.printBottom(String(settings.delayBefore) + "s");
        }
        if(currentScreen == Exposure)
        {
            M5_display.printTop("Exposure time");
            M5_display.printBottom(String(settings.lightTime) + "s");
        }
        if(currentScreen == Connection)
        {
            M5_display.printTop("Connection");

            if(settings.connectionMode == settings.Bluetooth)
            {
                if(canon_ble.isConnected())
                {
                    M5_display.printBottom(canon_ble.getPairedAddressString());
                }
                else
                {
                    M5_display.printBottom("Start pairing with A...");
                }            
            }
            else
            {
                M5_display.printBottom("Cable");
            }
        }

        currentScreen = nextScreen;
    }
}