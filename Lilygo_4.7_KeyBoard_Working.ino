#include <Arduino.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_driver.h"
#include "firasans.h"
#include <Wire.h>
#include <touch.h>
#include <math.h>

#if defined(CONFIG_IDF_TARGET_ESP32)
#define TOUCH_SCL   14
#define TOUCH_SDA   15
#define TOUCH_INT   13
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define TOUCH_SCL   17
#define TOUCH_SDA   18
#define TOUCH_INT   47
#endif
TouchClass touch;
uint8_t *framebuffer = NULL;

int cursor_x = 20;
int cursor_y = 60;

Rect_t area1 = {
    .x = 10,
    .y = 20,
    .width = EPD_WIDTH - 20,
    .height =  EPD_HEIGHT / 2 + 80
};
uint8_t state = 1;
uint8_t buf[2] = {0xD1, 0X05};

const char keyboard[] = {
    "   q   w   e   r   t   y   u   i   o   p   '\n"\
    "      a   s   d   f   g   h   j   k   l   .\n"\
    "         z    x   c   v   b   n  m   ,\n"\
};

char keyboardArray[] = {'q','\0','w','\0','e','\0','r','\0','t','\0','y','\0','u','\0','i','\0','o','\0','p','\0','\'','\0','a','\0','s','\0','d','\0','f','\0','g','\0','h','\0','j','\0','k','\0','l','\0','.','\0','z','\0','x','\0','c','\0','v','\0','b','\0','n','\0','m','\0',',','\0',' ','\0'};
//I want to cry^^^

void setup() {
  Serial.begin(115200);
    epd_init();

    pinMode(TOUCH_INT, INPUT_PULLUP);

    Wire.begin(TOUCH_SDA, TOUCH_SCL);

    if (!touch.begin()) {
        Serial.println("start touchscreen failed");
        while (1);
    }
    Serial.println("Started Touchscreen poll...");

    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer) {
        Serial.println("alloc memory failed !!!");
        while (1);
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    epd_poweron();
    epd_clear();
    //epd_clear();
    delay(10);


//Keyboard------
    cursor_x = 180;
    cursor_y = 375;
    write_string((GFXfont *)&FiraSans, (char *)keyboard, &cursor_x, &cursor_y, framebuffer);
    //epd_draw_rect(360, 490, 200, 40, 0, framebuffer);//spacebar
    cursor_x = 335;
    cursor_y = 520;
    writeln((GFXfont *)&FiraSans, "[                      ]", &cursor_x, &cursor_y, framebuffer);
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    epd_poweroff();
    /*
    cursor_x = 200;
    cursor_y = 400;
    for(int i = 0; i < 11; i++){
      epd_draw_rect(cursor_x + 25 + (i * 55), cursor_y - 38, 45, 50, 0, framebuffer);//epd_draw_rect( x start, y start, width, hight, color(0), framebuffer);
    }
    for(int i = 0; i < 10; i++){
      epd_draw_rect(cursor_x + 52 + (i * 52), cursor_y + 14, 45, 50, 0, framebuffer);//epd_draw_rect( x start, y start, width, hight, color(0), framebuffer);
    }
    for(int i = 0; i < 8; i++){
      epd_draw_rect(cursor_x + 92 + (i * 55), cursor_y + 66, 45, 50, 0, framebuffer);//epd_draw_rect( x start, y start, width, hight, color(0), framebuffer);
    }*/ //these are hit boxes for all the keys
    
//keyboard------
    cursor_x = 20;
    cursor_y = 30;
}

void loop() {
    const char* text;
    uint16_t  x, y;
    int column;
    if (digitalRead(TOUCH_INT)) {
        if (touch.scanPoint()) {
            touch.getPoint(x, y, 0);
            Serial.println(x);
            Serial.println(y);
            if(y > 170){
              column = round((x - 210) / 50);
              if(column < 0){column = 0;}
              else if(column > 10){column = 10;}
              column = column * 2;
              text = &keyboardArray[column];
            }
            else if(y > 110 && y < 171){
              column = round((x - 250) / 50);
              if(column < 0){column = 0;}
              else if(column > 9){column = 9;}
              column = column * 2; 
              text = &keyboardArray[column + 22];
            }
            else if(y > 50 && y < 111){
              column = round((x - 280) / 50);
              if(column < 0){column = 0;}
              else if(column > 7){column = 7;}
              column = column * 2; 
              text = &keyboardArray[column + 42];
            }
            else if(y > 0 && y < 51){
              column = 1;
              text = &keyboardArray[1];
              write_string((GFXfont *)&FiraSans, " ", &cursor_x, &cursor_y, NULL);
            }
     
            Serial.println(column);
            
            epd_poweron();
            write_string((GFXfont *)&FiraSans, (char *)text, &cursor_x, &cursor_y, NULL);//(char *)txt
            delay(5);
            epd_poweroff();
            cursor_y = 30;
        }
    }
    delay(5);
}
