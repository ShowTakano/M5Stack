// https://karakuri-musha.com/inside-technology/arduino-m5stack-microsd-readtext-01/
#include <M5Core2.h>

void setup() {
  M5.begin();                                               // M5Stack の初期化
  Serial.begin(115200);                                     // シリアル接続の初期化

  M5.Lcd.setTextFont(4);                                    // テキストフォント：26ピクセルASCIIフォント
  M5.Lcd.setTextSize(1);                                    // テキストサイズ

  if (!SD.begin()) {                                        // SDカードの初期化
    M5.Lcd.println("Card failed, or not present");          // SDカードが未挿入の場合の出力
    while (1);
  }
  M5.Lcd.println("microSD card initialized.");

  if ( SD.exists("/takano.txt")) {                        // ファイルの存在確認
    M5.Lcd.println("takano.txt exists.");                 // ファイルがある場合の処理

    delay(500);
    File myFile = SD.open("/takano.txt", FILE_READ);      // 読み取り専用でファイルを開く

    if (myFile) {                                           // ファイルが正常に開けた場合
      M5.Lcd.println("/takano.txt Content:");
      M5.Lcd.setCursor(0, 120);
      while (myFile.available()) {                          // ファイル内容を順にディスプレイに出力
        M5.Lcd.setTextColor(RED);
        M5.Lcd.write(myFile.read());
      }
      myFile.close();                                       // ファイルのクローズ
    } else {
      M5.Lcd.println("error opening /takano.txt");        // ファイルが開けない場合  
    }
  } else {
    M5.Lcd.println("takano.txt doesn't exit.");           // ファイルが存在しない場合
  }
}

void loop() {

}
