#include <Arduino.h>
#include <cstdlib>
#include <ctime>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Preferences.h> //for persistent storage :)
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

// enum Door{

//   door_1 = 0,
//   door_2 = 1,
//   door_3 = 2

// }

int randomPrizeDoor = -1;

bool inRandomDoor = true;
bool inIntroText = true;
bool inPhase_1 = true;
bool inOpenNonePrize = false;
bool resetScreen = true;
bool inChangeDialogue = false;
bool inchangeChoiceBox = false;
bool changeDoor = false;
bool noChangeDoor = false;

void setup() {

  Serial.begin(115200);

  srand(time(0));

  btnLeft.setPinMode();
  btnRight.setPinMode();
  btnOk.setPinMode();

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  
  // tft.drawLine(120, 0, 120, 240, ST77XX_WHITE); //Y-axis
  // tft.drawLine(0, 120, 240, 120, ST77XX_WHITE); //X-axis
  // tft.drawLine(0, 0, 240, 240, ST77XX_WHITE); //Diagonal up left to down right
  // tft.drawLine(0, 240, 240, 0, ST77XX_WHITE); //Diagonal down left to up right

}

void loop() {

  static int firstChoice;
  static int nonePrizeDoor;

  if(resetScreen){

    drawDoor_1();
    drawDoor_2();
    drawDoor_3();  

    drawChoiceBoxs();

    resetScreen = false;

  }

  //if(inIntroText) introText(); Remove the comment later

  if(inRandomDoor) randomPrizeDoor = randomDoor();

  if(inPhase_1) firstChoice = phase_1();

  if(inOpenNonePrize){

    loading(3000);

    nonePrizeDoor = openNonePrize(firstChoice, randomPrizeDoor);

    inChangeDialogue = true;

  }

  if(inChangeDialogue){
  
    changeChoiceDialogue();

    inchangeChoiceBox = true;
  
  }

  if(inchangeChoiceBox) changeDoor = changeChoiceBox();

  if(changeDoor){
    
    changeChoice(firstChoice, nonePrizeDoor, randomPrizeDoor);

  }

  if(noChangeDoor){

  if(firstChoice == randomPrizeDoor){

    openDoor(firstChoice);
    drawCoin(firstChoice);

    Serial.println("yaaaaaas win");

    //loading(3000);

    //store the fact that you changed choice and the win and increment total games -----------------------------------------

    noChangeDoor = false;
    resetAll(firstChoice, nonePrizeDoor);

  }

  else if(firstChoice != randomPrizeDoor){

    Serial.println("noooo lose");
    
    //loading(3000);

    noChangeDoor = false;
    resetAll(firstChoice, nonePrizeDoor);

  }

}

  //int changeChoice(firstChoice, randomPrizeDoor); new function, complete it

  // tft.setCursor(5, 190);
  // tft.setTextSize(2);
  // tft.setTextColor(ST77XX_RED);
  // tft.print("After that door opened/nWould you like to change/nyour choice?");
    
  btnLeft.update();
  btnRight.update();
  btnOk.update();

  // if(holdOn(3000)){

  //   Serial.print("first choice = ");
  //   Serial.println(firstChoice);
  //   Serial.print("Random prize door = ");
  //   Serial.println(randomPrizeDoor);
  //   Serial.print("None prize door = ");
  //   Serial.println(nonePrizeDoor);

  // }

}

int chooseDoor(){

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

int phase_1(){

  static int firstDoorChoice;

  firstDoorChoice = chooseDoor();

  if(btnOk.pressed()){

    ChoiceMarker(firstDoorChoice);
    inPhase_1 = false;
    inOpenNonePrize = true;
    return firstDoorChoice;

  }

  return -1;

}

void ChoiceMarker(int choice){

  switch (choice) {
  
    case 0: tft.setCursor(25, 133); tft.fillRect(115, 133, 10, 10, ST77XX_BLACK); tft.fillRect(205, 133, 10, 10, ST77XX_BLACK); break;
    case 1: tft.setCursor(115, 133); tft.fillRect(25, 133, 10, 10, ST77XX_BLACK); tft.fillRect(205, 133, 10, 10, ST77XX_BLACK); break;
    case 2: tft.setCursor(205, 133); tft.fillRect(115, 133, 10, 10, ST77XX_BLACK); tft.fillRect(25, 133, 10, 10, ST77XX_BLACK); break;
    default: break;

  }

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.write("^");

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

  tft.fillRect(15, 90 - 40, 30, 60, 0x1861); tft.fillCircle(35, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(15, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(15, 90 - 40, 30/2, 60, 0x1861); tft.fillCircle(35-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(15, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(15, 90 - 40, 2, 60, 0x1861);

  delay(200);
  
}

void openDoor2(){

  tft.fillRect(105, 90 - 40, 30, 60, 0x1861); tft.fillCircle(125, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(105, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(105, 90 - 40, 30/2, 60, 0x1861); tft.fillCircle(125-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(105, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(105, 90 - 40, 2, 60, 0x1861);

  delay(200);
  
}

void openDoor3(){

  tft.fillRect(195, 90 - 40, 30, 60, 0x1861); tft.fillCircle(215, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(195, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(195, 90 - 40, 30/2, 60, 0x1861); tft.fillCircle(215-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(195, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(195, 90 - 40, 2, 60, 0x1861);

  delay(200);
  
}

void pickDoor1(){

  tft.fillRect(27, 162 - 40, 6, 6, ST77XX_CYAN);

}

void pickDoor2(){

  tft.fillRect(117, 162 - 40, 6, 6, ST77XX_CYAN);

}

void pickDoor3(){

  tft.fillRect(207, 162 - 40, 6, 6, ST77XX_CYAN);  

}

void unpickDoor1(){

  tft.fillRect(27, 162 - 40, 6, 6, ST77XX_BLACK);

}

void unpickDoor2(){

  tft.fillRect(117, 162 - 40, 6, 6, ST77XX_BLACK);

}

void unpickDoor3(){

  tft.fillRect(207, 162 - 40, 6, 6, ST77XX_BLACK);  

}

static bool holdOn(int time){

  static unsigned long past = millis();

  if(millis() - past >= time){
    past = millis();
    return true;
  }

  return false;

}

void openDoor(int num){

  switch (num) {
  
    case 0: openDoor1(); break;
    case 1: openDoor2(); break;
    case 2: openDoor3(); break;
    default: break;

  }

}

void drawCoin(int num){

  switch(num){

    case 0: drawCoin_1(); break;
    case 1: drawCoin_2(); break;
    case 2: drawCoin_3(); break;
    default: break;

  }

}

void drawCoin_1(){

  tft.fillCircle(30, 125 - 40, 10, ST77XX_YELLOW);
  tft.setCursor(25, 121 - 40);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawCoin_2(){

  tft.fillCircle(120, 125 - 40, 10, ST77XX_YELLOW);
  tft.setCursor(115, 121 - 40);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawCoin_3(){

  tft.fillCircle(210, 125 - 40, 10, ST77XX_YELLOW);
  tft.setCursor(205, 121 - 40);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawDoor_1(){

  //Door frame                        ; Door knob                               ; Lower Rectangle                             ; Upper rectangle
  tft.fillRect(15, 90 - 40, 30, 60, 0x1861); tft.fillCircle(35, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22, 25, ST77XX_YELLOW);

}

void drawDoor_2(){

  //Door frame                         ; Door knob                                ; Lower Rectangle                              ; Upper rectangle
  tft.fillRect(105, 90 - 40, 30, 60, 0x1861); tft.fillCircle(125, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22, 25, ST77XX_YELLOW);

}

void drawDoor_3(){

  //Door frame                         ; Door knob                                ; Lower Rectangle                              ; Upper rectangle
  tft.fillRect(195, 90 - 40, 30, 60, 0x1861); tft.fillCircle(215, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22, 25, ST77XX_YELLOW);

}

void drawChoiceBoxs(){

  tft.drawRect(25, 160 - 40, 10, 10, ST77XX_GREEN);
  tft.drawRect(115, 160 - 40, 10, 10, ST77XX_GREEN);
  tft.drawRect(205, 160 - 40, 10, 10, ST77XX_GREEN);

}

int randomDoor(){

  int randomNum = rand() % 3;

  inRandomDoor = false;

  return randomNum;

}

void introText(){

  tft.setCursor(5, 120);
  tft.setTextSize(2);
  tft.print("Welcome to monty's hall game!");

  delay(5000);

  tft.setCursor(5, 120);
  tft.setTextSize(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.print("Behind one of three doors there is a \n prize");

  delay(5000);

  tft.setCursor(5, 120);
  tft.setTextSize(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.print("Choose the correct one to win");

  delay(5000);

  tft.fillScreen(ST77XX_BLACK);
  inIntroText = false;

}

int openNonePrize(int firstDoorChoice, int prizeDoor){

  int nonePrizeDoor;

  if(firstDoorChoice == prizeDoor){

    do{

      nonePrizeDoor = rand() % 3;

    }while(nonePrizeDoor == prizeDoor);

    openDoor(nonePrizeDoor);
    inOpenNonePrize = false;
    return nonePrizeDoor;

  }

  nonePrizeDoor = 3 - firstDoorChoice - prizeDoor;

  openDoor(nonePrizeDoor);
  inOpenNonePrize = false;
  return nonePrizeDoor;

}

void loading(int time){

  int holdTime = time/4;

  tft.fillRect(85, 200, 10, 10, ST77XX_WHITE);
  delay(holdTime);
  tft.fillRect(105, 200, 10, 10, ST77XX_WHITE);
  delay(holdTime);
  tft.fillRect(125, 200, 10, 10, ST77XX_WHITE);
  delay(holdTime);
  tft.fillRect(145, 200, 10, 10, ST77XX_WHITE);
  delay(holdTime);
    
  tft.fillRect(85, 200, 70, 10, ST77XX_BLACK);
  delay(500);

}

void resetAll(int &firstChoice, int &nonePrizeDoor){

  tft.fillScreen(ST77XX_BLACK);
  inRandomDoor = true;
  inPhase_1 = true;
  resetScreen = true;
  firstChoice = -1;
  nonePrizeDoor = -2;

}

void changeChoice(int &firstChoice, int &nonePrizeDoor, int prizeDoor){

  if(firstChoice != prizeDoor){

    ChoiceMarker(prizeDoor);
    openDoor(prizeDoor);
    drawCoin(prizeDoor);

    Serial.println("yaaaaaas win");

    changeDoor = false;

    resetAll(firstChoice, nonePrizeDoor);

  }

  else if(firstChoice == prizeDoor){

    int unknownDoor = 3 - firstChoice - nonePrizeDoor;

    Serial.println("noooo lose");

    ChoiceMarker(unknownDoor);
    changeDoor = false;

    resetAll(firstChoice, nonePrizeDoor);

  }

}

bool changeChoiceBox(){

  static int choice = 1;

  if(btnLeft.pressed()){choice--; if(choice < 0){choice = 1;};}
  else if(btnRight.pressed()){choice++; if(choice > 1){choice = 0;};}

  switch(choice){

    case 0: tft.fillRect(35, 215, 5, 5, ST77XX_GREEN); tft.fillRect(82, 215, 5, 5, ST77XX_BLACK); break;
    case 1: tft.fillRect(82, 215, 5, 5, ST77XX_GREEN); tft.fillRect(35, 215, 5, 5, ST77XX_BLACK); break;
    default: break;

  }

  btnOk.update();

  if(btnOk.pressed()){

    tft.invertDisplay(true);
    inchangeChoiceBox = false;
    switch(choice){

      case 0: Serial.print("YES change DOOR: "); return true; break;
      case 1: noChangeDoor = true; Serial.print("NO change DOOR: "); return false; break;
      default: break;

    }

  }

  return NULL;

}

void changeChoiceDialogue(){

  tft.setCursor(5, 170);
  tft.setTextSize(1);
  tft.print("Would you like to change your choice?");

  tft.setCursor(30, 200);
  tft.print("yes");
  tft.drawRect(35, 215, 7, 7, ST77XX_GREEN);

  tft.setCursor(80, 200);
  tft.print("no");
  tft.drawRect(82, 215, 7, 7, ST77XX_GREEN);

  inChangeDialogue = false;

}
