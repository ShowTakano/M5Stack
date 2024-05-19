#include <M5Stack.h>

void setup(){
  // 初期設定
  M5.begin();

  // 表示1
  M5.Lcd.setTextColor(TFT_WHITE); // 色コードを指定
  M5.Lcd.setTextSize(1); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 0, 1);

  // 表示2
  M5.Lcd.setTextColor(TFT_YELLOW); // 色コードを指定
  M5.Lcd.setTextSize(2); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 8, 1);

  // 表示3
  M5.Lcd.setTextColor(TFT_RED); // 色コードを指定
  M5.Lcd.setTextSize(3); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 22, 1);

  // 表示4
  M5.Lcd.setTextColor(TFT_GREEN); // 色コードを指定
  M5.Lcd.setTextSize(4); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 43, 1);

  // 表示5
  M5.Lcd.setTextColor(TFT_DARKGREEN); // 色コードを指定
  M5.Lcd.setTextSize(5); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 71, 1); 

  // 表示6
  M5.Lcd.setTextColor(TFT_BLUE); // 色コードを指定
  M5.Lcd.setTextSize(6); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 108, 1); 

  // 表示7（縦が画面外に出るので表示されない？）
  M5.Lcd.setTextColor(TFT_BLACK); // 色コードを指定
  M5.Lcd.setTextSize(7); // 1〜7
  M5.Lcd.drawString("Hello World", 0, 150, 1);
}

void loop() {

}
