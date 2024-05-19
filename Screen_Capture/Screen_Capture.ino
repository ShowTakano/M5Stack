//M5Stack screen capture sample project Copyright (c) 2022- @logic_star
// https://www.marutsu.co.jp/pc/static/large_order/capture_220812
#define CORE2 //CORE2を使用する場合は有効化要
#ifdef CORE2
  #include <M5Core2.h>
#else
  #include <M5Stack.h>
#endif
#include "Capture.h"

int counter;
char fn[100];

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextSize(1);
  counter = 0;
}

void loop() {
  M5.update();
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.printf("Counter = %d", counter);
  if(M5.BtnA.isPressed()){
    sprintf(fn, "/Counter%d.bmp", counter);
    Screen_Capture_BMP(fn);
  }
  if(M5.BtnC.isPressed()) while(1);
  counter++;
  delay(500);  //0.5秒ウェイト
}
