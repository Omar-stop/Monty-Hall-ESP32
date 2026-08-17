#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

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

/*enum class Door1_Frame{

    x= 15,
    y= 90,
    w= 30,
    h= 60,
    color= 0x1861

  };

enum class Door1_Knob{

  x = 20,
  y = 127,
  r = 2.5,
  color = ST77XX_WHITE

  };
enum class Door1_LowerRect{

  x = 19,
  y = 135,
  w = 22,
  h = 12,
  color = ST77XX_YELLOW

  };
enum class Door1_UpperRect{

  x = 19,
  y = 95,
  w = 22,
  h = 25,
  color = ST77XX_YELLOW

  };
enum class Door2_Frame{

    x= 105,
    y= 90,
    w= 30,
    h= 60,
    color= 0x1861

  };

enum class Door2_Knob{

  x = 110,
  y = 127,
  r = 2.5,
  color = ST77XX_WHITE

  };
enum class Door2_LowerRect{

  x = 109,
  y = 135,
  w = 22,
  h = 12,
  color = ST77XX_YELLOW

  };
enum class Door2_UpperRect{

  x = 109,
  y = 95,
  w = 22,
  h = 25,
  color = ST77XX_YELLOW

  };
enum class Door3_Frame{

    x= 195,
    y= 90,
    w= 30,
    h= 60,
    color= 0x1861

  };

enum class Door3_Knob{

  x = 200,
  y = 127,
  r = 2.5,
  color = ST77XX_WHITE

  };
enum class Door3_LowerRect{

  x = 199,
  y = 135,
  w = 22,
  h = 12,
  color = ST77XX_YELLOW

  };
enum class Door3_UpperRect{

  x = 199,
  y = 95,
  w = 22,
  h = 25,
  color = ST77XX_YELLOW

  };*/

void setup() {

  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  tft.drawLine(120, 0, 120, 240, ST77XX_WHITE); //Y-axis
  tft.drawLine(0, 120, 240, 120, ST77XX_WHITE); //X-axis
  tft.drawLine(0, 0, 240, 240, ST77XX_WHITE); //Diagonal up left to down right
  tft.drawLine(0, 240, 240, 0, ST77XX_WHITE); //Diagonal down left to up right

  //Door frame                        ; Door knob                                 ; Lower Rectangle                             ; Upper rectangle
  tft.fillRect(15, 90, 30, 60, 0x1861); tft.fillCircle(20, 127, 2.5, ST77XX_WHITE); tft.drawRect(19, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(19, 95, 22, 25, ST77XX_YELLOW);
  tft.fillRect(105, 90, 30, 60, 0x1861); tft.fillCircle(110, 127, 2.5, ST77XX_WHITE); tft.drawRect(109, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22, 25, ST77XX_YELLOW);
  tft.fillRect(195, 90, 30, 60, 0x1861); tft.fillCircle(200, 127, 2.5, ST77XX_WHITE); tft.drawRect(199, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22, 25, ST77XX_YELLOW);
  
  tft.setCursor(5, 220);
  tft.setTextSize(2);
  tft.print("-Choose any door");
  
}

void loop() {

  //openDoor1();

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

  tft.fillScreen(ST77XX_BLACK);


  tft.fillRect(15, 90, 30/2, 60, 0x1861); tft.fillCircle(35-12, 127, 1, ST77XX_WHITE); tft.drawRect(19, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(19, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillScreen(ST77XX_BLACK);

  tft.fillRect(15, 90, 2, 60, 0x1861);

  delay(200);
  
}

void openDoor2(){

  tft.fillRect(105, 90, 30, 60, ST77XX_BLUE); tft.fillCircle(125, 127, 2.5, ST77XX_WHITE); tft.drawRect(109, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillScreen(ST77XX_BLACK);


  tft.fillRect(105, 90, 30/2, 60, ST77XX_BLUE); tft.fillCircle(125-12, 127, 1, ST77XX_WHITE); tft.drawRect(109, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(109, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillScreen(ST77XX_BLACK);

  tft.fillRect(105, 90, 2, 60, ST77XX_BLUE);

  delay(200);
  
}

void openDoor3(){

  tft.fillRect(195, 90, 30, 60, ST77XX_BLUE); tft.fillCircle(215, 127, 2.5, ST77XX_WHITE); tft.drawRect(199, 135, 22, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22, 25, ST77XX_YELLOW);

  delay(200);

  tft.fillScreen(ST77XX_BLACK);


  tft.fillRect(195, 90, 30/2, 60, ST77XX_BLUE); tft.fillCircle(215-12, 127, 1, ST77XX_WHITE); tft.drawRect(199, 135, 22/3, 12, ST77XX_YELLOW); tft.drawRect(199, 95, 22/3, 25, ST77XX_YELLOW);

  delay(100);

  tft.fillScreen(ST77XX_BLACK);

  tft.fillRect(195, 90, 2, 60, ST77XX_BLUE);

  delay(200);
  
}
