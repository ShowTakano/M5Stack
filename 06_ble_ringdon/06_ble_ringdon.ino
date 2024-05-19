#include <M5Core2.h>
#include <WiFi.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define JST 3600 * 9        // グリニッジ標準時と日本標準時の差

char *ssid = "F660A-fGG2-G";      // Wi-FiのSSID
char *password = "3ccqkz57";  // Wi-Fiのパスワード

const char* week[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  // 曜日を定義

int TEXT_SIZE = 2;
int CURSOR_HEIGHT = 26;
int CURSOR_Y_POS = 0;

String data;

////////////////////////////////////////////////////////////////////////////////
// Wi-Fi設定
////////////////////////////////////////////////////////////////////////////////
void setup_wifi() {
  // Wi-Fi接続をリセット
  WiFi.disconnect();
  delay(500);
  WiFi.begin(ssid, password);
  // Wi-Fi接続待ち
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

////////////////////////////////////////////////////////////////////////////////
// 現在時刻（現地時間）を表示
////////////////////////////////////////////////////////////////////////////////
void printLocalTime() {
  struct tm timeinfo;
  // 時刻を取得できなかった場合
  if (!getLocalTime(&timeinfo)) {
    M5.Lcd.printf("Failed to obtain time. ");
    return;
  }
  // 現在時刻を表示
  M5.Lcd.printf("%04d\/%02d\/%02d(%s) ", \
    timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, week[timeinfo.tm_wday]);
  M5.Lcd.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

float get_bat_voltage() {
  // get bat boltage
  float batVoltage = M5.Axp.GetBatVoltage();
  float batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;
  return batVoltage;
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(TEXT_SIZE);
  
  // Wi-Fi設定
  setup_wifi();
  
  // NTPサーバとローカルのタイムゾーンを設定
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  // bluetooth
  SerialBT.begin("M5Stack core2.");

  // DingDong
  M5.Spk.begin();
  M5.Spk.DingDong();
}

void loop() {
  M5.Lcd.setCursor(0, 0);
  // 現在時刻を表示
  printLocalTime();

  // バッテリ電圧を表示
  M5.Lcd.setCursor(0, 26);
  float bat_v = get_bat_voltage();
  M5.Lcd.printf("bat voltage : %f V", bat_v);
  
  // bluetooth
  if ( SerialBT.available() ){
    data = SerialBT.readStringUntil('\n');
    M5.Spk.DingDong();
  }

  // left touch
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.y > 240 && pos.x < 109) {
    M5.Spk.DingDong();
  }

  // right touch
  if (pos.y > 240 && pos.x > 218) {
    data = "1";
    SerialBT.println(data);
    M5.Spk.DingDong();
  }
}
