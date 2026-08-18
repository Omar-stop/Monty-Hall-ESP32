#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "button.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

#define TFT_SCLK 18
#define TFT_MOSI 23
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 4
#define TFT_BL 27

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define YELLOW 0xFFE0

#define btnLeftPin 32
#define btnRightPin 33
#define btnOkPin 26

Button btnLeft(btnLeftPin);
Button btnRight(btnRightPin);
Button btnOk(btnOkPin);

void setup() {

  Serial.begin(115200);

  btnLeft.setPinMode();
  btnRight.setPinMode();
  btnOk.setPinMode();

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(120, 120);
  tft.setTextSize(2);
  tft.print("Welcome to Monty Hall's problem!");
  delay(2000);

  tft.fillScreen(ST77XX_BLACK);
  
  tft.drawLine(120, 0, 120, 240, ST77XX_WHITE); //Y-axis
  tft.drawLine(0, 120, 240, 120, ST77XX_WHITE); //X-axis
  tft.drawLine(0, 0, 240, 240, ST77XX_WHITE); //Diagonal up left to down right
  tft.drawLine(0, 240, 240, 0, ST77XX_WHITE); //Diagonal down left to up right

  //Door frame                        ; Door knob                                 ; Lower Rectangle                             ; Upper rectangle
  tft.fillRect(15, 90, 30, 60, 0x1861); tft.fillCircle(35, 127, 2, ST77XX_WHITE); tft.drawRect(19, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95, 22, 25, ST77XX_YELLOW);
  tft.fillRect(105, 90, 30, 60, 0x1861); tft.fillCircle(125, 127, 2, ST77XX_WHITE); tft.drawRect(109, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22, 25, ST77XX_YELLOW);
  tft.fillRect(195, 90, 30, 60, 0x1861); tft.fillCircle(215, 127, 2, ST77XX_WHITE); tft.drawRect(199, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22, 25, ST77XX_YELLOW);

  tft.drawRect(25, 160, 10, 10, ST77XX_GREEN);
  tft.drawRect(115, 160, 10, 10, ST77XX_GREEN);
  tft.drawRect(205, 160, 10, 10, ST77XX_GREEN);
  
  tft.setCursor(5, 220);
  tft.setTextSize(2);
  tft.print("-Choose any door");
  
}

void loop() {

  int firstDoorChoice = firstChoice();
  if(btnOk.pressed())openDoor(firstDoorChoice);

  btnLeft.update();
  btnRight.update();
  btnOk.update();

  //1-Next draw the gold shape, 2-Function: generate a random number for the winning door, 3-Function: opens an empty door or any door if 
  //the golden door was chosen correctly first try.

}

//Cool function I found on a wokwi project
void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void openDoor1(){

  tft.fillRect(15, 90, 30, 60, 0x1861); tft.fillCircle(35, 127, 2.5, ST77XX_WHITE); tft.drawRect(19, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(15, 90, 30, 60, ST77XX_BLACK);


  tft.fillRect(15, 90, 30/2, 60, 0x1861); tft.fillCircle(35-12, 127, 1, ST77XX_WHITE); tft.drawRect(19, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(19, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(15, 90, 30, 60, ST77XX_BLACK);

  tft.fillRect(15, 90, 2, 60, 0x1861);

  delay(200);
  
}

void openDoor2(){

  tft.fillRect(105, 90, 30, 60, 0x1861); tft.fillCircle(125, 127, 2.5, ST77XX_WHITE); tft.drawRect(109, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(105, 90, 30, 60, ST77XX_BLACK);


  tft.fillRect(105, 90, 30/2, 60, 0x1861); tft.fillCircle(125-12, 127, 1, ST77XX_WHITE); tft.drawRect(109, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(105, 90, 30, 60, ST77XX_BLACK);

  tft.fillRect(105, 90, 2, 60, 0x1861);

  delay(200);
  
}

void openDoor3(){

  tft.fillRect(195, 90, 30, 60, 0x1861); tft.fillCircle(215, 127, 2.5, ST77XX_WHITE); tft.drawRect(199, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(195, 90, 30, 60, ST77XX_BLACK);


  tft.fillRect(195, 90, 30/2, 60, 0x1861); tft.fillCircle(215-12, 127, 1, ST77XX_WHITE); tft.drawRect(199, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(195, 90, 30, 60, ST77XX_BLACK);

  tft.fillRect(195, 90, 2, 60, 0x1861);

  delay(200);
  
}

void pickDoor1(){

  tft.fillRect(27, 162, 6, 6, ST77XX_CYAN);

}

void pickDoor2(){

  tft.fillRect(117, 162, 6, 6, ST77XX_CYAN);

}

void pickDoor3(){

  tft.fillRect(207, 162, 6, 6, ST77XX_CYAN);  

}

void unpickDoor1(){

  tft.fillRect(27, 162, 6, 6, ST77XX_BLACK);

}

void unpickDoor2(){

  tft.fillRect(117, 162, 6, 6, ST77XX_BLACK);

}

void unpickDoor3(){

  tft.fillRect(207, 162, 6, 6, ST77XX_BLACK);  

}

static bool holdOn(int time){

  static unsigned long past = millis();

  if(millis() - past >= time){
    past = millis();
    return true;
  }

  return false;

}

int firstChoice(){

  static int choice = 0;

  if(btnLeft.pressed()){choice--; if(choice < 0){choice = 2;};}
  else if(btnRight.pressed()){choice++; if(choice > 2){choice = 0;};}

  switch(choice){

    case 0: pickDoor1(); unpickDoor2(); unpickDoor3(); break;
    case 1: pickDoor2(); unpickDoor1(); unpickDoor3(); break;
    case 2: pickDoor3(); unpickDoor2(); unpickDoor1(); break;
    default: unpickDoor1(); unpickDoor2(); unpickDoor3(); break;

  }

  if(btnOk.pressed())return choice;

}

void openDoor(int num){

  switch (num) {
  
    case 0: tft.fillRect(15, 90, 30, 60, ST77XX_BLACK); openDoor1(); break;
    case 1: tft.fillRect(105, 90, 30, 60, ST77XX_BLACK); openDoor2(); break;
    case 2: tft.fillRect(195, 90, 30, 60, ST77XX_BLACK); openDoor3(); break;
    default: break;

  }

}


