#include <SPI.h>
#include <M5Core2.h>
#include "Adafruit_MAX31855.h"

// 下記では、LCDを除けば正常に機能した。しかしLCD上に文字を書くことなどが機能しなかった。
// #define MAXDO   38  // MISO38
// #define MAXCS   19  // G19
// #define MAXCLK  18  // SCK18
// 内部でLCDとのSPI通信に18,23,38が使用されているようで、干渉しておかしな挙動のようだ。

// 自由に使えるのは 13,14,19,25,26,27 ただし無線利用時とアナログはダメ等に注意。毎度調べたほうが良い。

// VSPIではなくHSPIという方法であれば可能との情報アリ。また干渉を避けるため下記のピンとした。
#define CK 13
#define MI 14
#define MO 25
#define SS 19
// これでHSPIで温度取得成功。

// しかし、結局、adafruitの方法でも干渉さえしなければLCDも正常に機能した。
#define MAXDO   14
#define MAXCS   19
#define MAXCLK  13

// #define HSPI_CLK 1000000
// SPIClass hspi(HSPI);
// SPISettings spiSettings = SPISettings(HSPI_CLK,SPI_MSBFIRST,SPI_MODE0);

float bat_v = 0.0;
float bat_p = 0.0;
float temp = 0.0;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void get_bat_status() {
  bat_v = M5.Axp.GetBatVoltage();
  bat_p = ( bat_v < 3.2 ) ? 0 : ( bat_v - 3.2 ) * 100;
}

void init_lcd() {
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.fillScreen(BLACK);
}

double get_temp() {
   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Thermocouple fault(s) detected!");
     uint8_t e = thermocouple.readError();
     if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
     if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
     if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
   } else {
     temp = float(c);
     Serial.print("temp = ");
     Serial.print(temp);
     Serial.println(" C");
   }
  return temp;
}

// void measurement() {
//   int32_t rdata;
//   hspi.beginTransaction(spiSettings);
//   digitalWrite(SS,LOW);
//   rdata = hspi.transfer32(0xFF);
//   digitalWrite(SS,HIGH);
//   hspi.endTransaction();

//   double internal = (rdata >> 4) & 0x7FFF;
//   internal *= 0.0625;
//   if((rdata >> 4) & 0x800) internal *= -1;
//   if( rdata & 0x80000000){
//     rdata = 0xFFFFC000 | ((rdata >> 18) & 0x00003FFF);
//   }else{
//     rdata >>= 18;
//   }
//   double temperature;
//   temperature = rdata;
//   temperature *= 0.25;

//   Serial.println(temperature);
// }

void check_touch_button() {
  // left touch
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.y > 240 && pos.x < 109) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);
  }
  // center touch
  if (pos.y > 240 && 109 < pos.x && pos.x < 218) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);

  }
  // right touch
  if (pos.y > 240 && pos.x > 218) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);
  }
}

void show_lcd_bottom() {
  M5.Lcd.setTextDatum(7);  // 中央下基準へ
  M5.Lcd.drawCentreString("Left", (M5.Lcd.width()/3)/2, M5.Lcd.height(), 4);  // font4=26px
  M5.Lcd.drawCentreString("Center", M5.Lcd.width()/2, M5.Lcd.height(), 4);
  M5.Lcd.drawCentreString("Right", M5.Lcd.width()-((M5.Lcd.width()/3)/2), M5.Lcd.height(), 4);
  M5.Lcd.setTextDatum(0);  // 左上基準に戻す  
}

void show_lcd_top() {
  // bat
  M5.Lcd.fillRect(0, 0, M5.Lcd.width(), 16, BLACK);
  M5.Lcd.setTextDatum(2);  // 右上基準へ
  M5.Lcd.drawRightString("bat: " + String(bat_v) + "(V) " + String(bat_p) + "(%)", M5.Lcd.width(), 0, 2);  // font2=16pix
  M5.Lcd.setTextDatum(0);  // 左上基準に戻す

  // temp
  M5.Lcd.drawString("Temp: " + String(temp) + "(C)", 0, 0, 2);  // font2=16pix

}
void show_lcd_init_chart() {
  int space = 10;
  M5.Lcd.fillRect(
    space,
    16 + space,
    M5.Lcd.width() - (16 + space),
    M5.Lcd.height() - (26 + 16 + space + space),
    M5.Lcd.color565(50, 50, 50)  // rgb
  );
}

void setup() {
  // M5
  M5.begin();
  delay(500);
  init_lcd();
  M5.Spk.begin();

  // HSPI
  // pinMode(CK,OUTPUT);
  // pinMode(MI,INPUT);
  // pinMode(MO,OUTPUT);
  // pinMode(SS,OUTPUT);
  // hspi.begin(CK,MI,MO,SS);

  // serial
  Serial.begin(9600);
  // while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  // Serial.print("Initializing sensor...");
  // if (!thermocouple.begin()) {
  //   Serial.println("ERROR.");
  //   while (1) delay(10);
  // }

  Serial.println("setup() DONE.");
}

void loop() {
  get_bat_status();
  get_temp();
  // measurement();
  check_touch_button();
  show_lcd_bottom();
  show_lcd_top();
  show_lcd_init_chart();
  delay(100);
}

