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

#define Brown 0x1861

#define btnLeftPin 32
#define btnRightPin 33
#define btnOkPin 26

Button btnLeft(btnLeftPin);
Button btnRight(btnRightPin);
Button btnOk(btnOkPin);

Preferences prefs;

struct {

  int Ttl_Change_Att = 0;
  int Ttl_NChange_Att = 0;
  int Change_Cnt_W = 0;
  int NChange_Cnt_W = 0;

} stats;

enum doorNum{

  door_1 = 0,
  door_2 = 1,
  door_3 = 2

};

int randomPrizeDoor = -1;

bool inRandomDoor = true;
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

  prefs.begin("stats", false);
  prefs.getBytes("stats", &stats, sizeof(stats));
  prefs.end();

  btnLeft.setPinMode();
  btnRight.setPinMode();
  btnOk.setPinMode();

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  //introText();  remove coment later

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

  if(inRandomDoor) randomPrizeDoor = randomDoor();

  if(inPhase_1) firstChoice = phase_1();

  if(inOpenNonePrize){

    loading(1000);

    nonePrizeDoor = openNonePrize(firstChoice, randomPrizeDoor);

    inChangeDialogue = true;

  }

  if(inChangeDialogue){
  
    changeChoiceDialogue();

    inchangeChoiceBox = true;
    btnOk.update();
  
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

      loading(1000);

      stats.NChange_Cnt_W++;

      noChangeDoor = false;
      resetAll(firstChoice, nonePrizeDoor);

    }

    else if(firstChoice != randomPrizeDoor){

      Serial.println("noooo lose");

      openDoor(firstChoice);
      loser(firstChoice);
      
      loading(1000);

      noChangeDoor = false;
      resetAll(firstChoice, nonePrizeDoor);

    }

  }

  btnLeft.update();
  btnRight.update();
  btnOk.update();

}

int chooseDoor(){

  static int choice = 0;
  static bool refresh = true;

  if(btnLeft.pressed() && holdOn(200)){choice--; refresh = true; if(choice < 0){choice = 2;};}
  else if(btnRight.pressed() && holdOn(200)){choice++; refresh = true; if(choice > 2){choice = 0;};}

  if(refresh){

    switch(choice){

      case door_1: pickDoor1(); unpickDoor2(); unpickDoor3(); break;
      case door_2: pickDoor2(); unpickDoor1(); unpickDoor3(); break;
      case door_3: pickDoor3(); unpickDoor2(); unpickDoor1(); break;
      default: unpickDoor1(); unpickDoor2(); unpickDoor3(); break;

    }

    refresh = false;

  }

  if(btnOk.pressed())return choice;

  return -1;

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
  
    case door_1: tft.setCursor(25, 133); tft.fillRect(115, 133, 10, 10, ST77XX_BLACK); tft.fillRect(205, 133, 10, 10, ST77XX_BLACK); break;
    case door_2: tft.setCursor(115, 133); tft.fillRect(25, 133, 10, 10, ST77XX_BLACK); tft.fillRect(205, 133, 10, 10, ST77XX_BLACK); break;
    case door_3: tft.setCursor(205, 133); tft.fillRect(115, 133, 10, 10, ST77XX_BLACK); tft.fillRect(25, 133, 10, 10, ST77XX_BLACK); break;
    default: break;

  }

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.write("^");

}

void openDoor1(){

  tft.fillRect(15, 90 - 40, 30, 60, Brown); tft.fillCircle(35, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(15, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(15, 90 - 40, 30/2, 60, Brown); tft.fillCircle(35-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(15, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(15, 90 - 40, 2, 60, Brown);

  delay(200);
  
}

void openDoor2(){

  tft.fillRect(105, 90 - 40, 30, 60, Brown); tft.fillCircle(125, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(105, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(105, 90 - 40, 30/2, 60, Brown); tft.fillCircle(125-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(105, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(105, 90 - 40, 2, 60, Brown);

  delay(200);
  
}

void openDoor3(){

  tft.fillRect(195, 90 - 40, 30, 60, Brown); tft.fillCircle(215, 127 - 40, 2.5, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillRect(195, 90 - 40, 30, 60, ST77XX_BLACK);


  tft.fillRect(195, 90 - 40, 30/2, 60, Brown); tft.fillCircle(215-12, 127 - 40, 1, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22/3, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillRect(195, 90 - 40, 30, 60, ST77XX_BLACK);

  tft.fillRect(195, 90 - 40, 2, 60, Brown);

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
  
    case door_1: openDoor1(); break;
    case door_2: openDoor2(); break;
    case door_3: openDoor3(); break;
    default: break;

  }

}

void drawCoin(int num){

  switch(num){

    case door_1: drawCoin_1(); break;
    case door_2: drawCoin_2(); break;
    case door_3: drawCoin_3(); break;
    default: break;

  }

}

void drawCoin_1(){

  tft.fillCircle(30, 85, 10, ST77XX_ORANGE);
  tft.fillCircle(30, 85, 7, ST77XX_YELLOW);
  tft.setCursor(25, 79);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawCoin_2(){

  tft.fillCircle(120, 85, 10, ST77XX_ORANGE);
  tft.fillCircle(120, 85, 7, ST77XX_YELLOW);
  tft.setCursor(115, 79);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawCoin_3(){

  tft.fillCircle(210, 85, 10, ST77XX_ORANGE);
  tft.fillCircle(210, 85, 7, ST77XX_YELLOW);
  tft.setCursor(205, 79);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.write("$");

}

void drawDoor_1(){

  //Door frame                            ; Door knob                                    ; Lower Rectangle                                  ; Upper rectangle
  tft.fillRect(15, 90 - 40, 30, 60, Brown); tft.fillCircle(35, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(19, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95 - 40, 22, 25, ST77XX_YELLOW);

}

void drawDoor_2(){

  //Door frame                             ; Door knob                                     ; Lower Rectangle                                   ; Upper rectangle
  tft.fillRect(105, 90 - 40, 30, 60, Brown); tft.fillCircle(125, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(109, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95 - 40, 22, 25, ST77XX_YELLOW);

}

void drawDoor_3(){

  //Door frame                             ; Door knob                                     ; Lower Rectangle                                   ; Upper rectangle
  tft.fillRect(195, 90 - 40, 30, 60, Brown); tft.fillCircle(215, 127 - 40, 2, ST77XX_WHITE); tft.drawRect(199, 135 - 40, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95 - 40, 22, 25, ST77XX_YELLOW);

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
  tft.print("Behind one of three\ndoors there is a\nprize");

  delay(5000);

  tft.setCursor(5, 120);
  tft.setTextSize(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.print("Choose the correct one to win");

  delay(5000);

  tft.fillScreen(ST77XX_BLACK);

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

  tft.fillRect(125, 210, 10, 10, ST77XX_RED);
  delay(holdTime);
  tft.fillRect(145, 210, 10, 10, ST77XX_WHITE);
  delay(holdTime);
  tft.fillRect(165, 210, 10, 10, ST77XX_BLUE);
  delay(holdTime);
  tft.fillRect(185, 210, 10, 10, ST77XX_ORANGE);
  delay(holdTime);
    
  tft.fillRect(125, 210, 70, 10, ST77XX_BLACK);
  delay(500);

}

void storeAll(){

  prefs.begin("stats", false);

  prefs.putBytes("stats", &stats, sizeof(stats));

  prefs.end();

}

void resetAll(int &firstChoice, int &nonePrizeDoor){

  tft.fillScreen(ST77XX_BLACK);

  inRandomDoor = true;
  inPhase_1 = true;
  resetScreen = true;

  firstChoice = -1;
  nonePrizeDoor = -2;

  storeAll();

  showStored();

}

void changeChoice(int &firstChoice, int &nonePrizeDoor, int prizeDoor){

  if(firstChoice != prizeDoor){

    ChoiceMarker(prizeDoor);
    openDoor(prizeDoor);
    drawCoin(prizeDoor);

    Serial.println("yaaaaaas win");

    loading(1000);

    stats.Change_Cnt_W++;

    changeDoor = false;

    resetAll(firstChoice, nonePrizeDoor);

  }

  else if(firstChoice == prizeDoor){

    int unknownDoor = 3 - firstChoice - nonePrizeDoor;

    Serial.println("noooo lose");

    ChoiceMarker(unknownDoor);
    openDoor(unknownDoor);
    loser(unknownDoor);

    loading(1000);

    changeDoor = false;

    resetAll(firstChoice, nonePrizeDoor);

  }

}

bool changeChoiceBox(){

  static int choice = 1;
  static bool refresh = true;

  if(btnLeft.pressed() && holdOn(200)){choice--; refresh = true; if(choice < 0){choice = 1;};}
  else if(btnRight.pressed() && holdOn(200)){choice++; refresh = true; if(choice > 1){choice = 0;};}

  if(refresh){

    switch(choice){

      case 0: tft.fillRect(37, 217, 3, 3, ST77XX_CYAN); tft.fillRect(83, 216, 4, 4, ST77XX_BLACK); break;
      case 1: tft.fillRect(84, 217, 3, 3, ST77XX_CYAN); tft.fillRect(36, 216, 4, 4, ST77XX_BLACK); break;
      default: break;

    }

    refresh = false;

  }

  if(btnOk.pressed()){

    inchangeChoiceBox = false;
    switch(choice){

      case 0: Serial.print("YES change DOOR: "); stats.Ttl_Change_Att++; return true; break;
      case 1: noChangeDoor = true; Serial.print("NO change DOOR: "); stats.Ttl_NChange_Att++; return false; break;
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

void showStored(){

  Serial.print("Ttl_Change_Att = ");
  Serial.println(stats.Ttl_Change_Att);

  Serial.print("Ttl_NChange_Att = ");
  Serial.println(stats.Ttl_NChange_Att);

  Serial.print("Change_Cnt_W = ");
  Serial.println(stats.Change_Cnt_W);

  Serial.print("NChange_Cnt_W = ");
  Serial.println(stats.NChange_Cnt_W);

}

void loser(int doorNum){

  switch (doorNum) {
    
    case door_1: tft.setCursor(22, 70); tft.setTextSize(2); tft.setTextColor(ST77XX_RED); tft.write(";-;"); break;
    case door_2: tft.setCursor(112, 70); tft.setTextSize(2); tft.setTextColor(ST77XX_RED); tft.write(";-;"); break;
    case door_3: tft.setCursor(202, 70); tft.setTextSize(2); tft.setTextColor(ST77XX_RED); tft.write(";-;"); break;
    default: break;

  }

}
