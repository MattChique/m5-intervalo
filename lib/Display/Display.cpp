#include "Display.h"
#include <Arduino.h>
#include "M5StickCPlus.h"

Display::Display(M5Display* tft, String name):
    buffer(tft),
    name(name)
{
    buffer.createSprite(240, 135);
    buffer.setTextColor((negatif?TFT_WHITE:TFT_BLACK));
    buffer.setCursor(0, 0);
}

void Display::set_init_screen()
{
    buffer.fillRect(0, 0, 240, 135, (negatif?TFT_BLACK:TFT_WHITE));
    
    buffer.setTextSize(1);
    buffer.setFreeFont(font_name);
    buffer.drawString(name, 120 - (buffer.textWidth(name)/2.0), 55); 

    buffer.pushSprite(0,0);
}

void Display::set_main_screen()
{
    buffer.drawLine(0, 28, 240, 28, (negatif?TFT_RED:TFT_BLACK));

    buffer.drawLine(0, 107, 240, 107, (negatif?TFT_RED:TFT_BLACK));

    buffer.pushSprite(0,0);
}

void Display::clear()
{
    buffer.fillRect(0, 0, 240, 135, (negatif?TFT_BLACK:TFT_RED));

    buffer.pushSprite(0,0);
}

void Display::printTop(String message)
{
    buffer.fillRect(0, 29, 240, 38, (negatif?TFT_BLACK:TFT_RED));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.drawString(message, 10, 44);

    buffer.pushSprite(0,0);
}

void Display::printBottom(String message)
{
    buffer.fillRect(0, 67, 240, 37, (negatif?TFT_BLACK:TFT_RED));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.drawString(message, 10, 80);

    buffer.pushSprite(0,0);
}

void Display::printBattery(String message)
{
    buffer.fillRect(180, 0, 60, 17, (negatif?TFT_BLACK:TFT_RED));

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(0.6);
    buffer.drawString(message, 180, 5);

    buffer.pushSprite(0,0);
}