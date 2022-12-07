#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "M5StickCPlus.h"
#include <Arduino.h>

class Display
{
    private:
        TFT_eSprite buffer;
        String name;
        String address;
        bool negatif = true;

        const GFXfont* font_name = &Yellowtail_32;
        const GFXfont* font_titles = &FreeSans24pt7b;
        const GFXfont* font_text = &FreeSans12pt7b;
        const GFXfont* font_text_small = &FreeSans9pt7b;
    public :
        Display(M5Display* tft, String name);
        void set_init_screen();
        void set_main_screen();
        void clear();
        void print(String message);
        void printTop(String message);
        void printBottom(String message);
        void printBattery(String message);        
};

#endif