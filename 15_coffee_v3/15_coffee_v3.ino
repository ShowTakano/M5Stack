#include <SPI.h>
#include <M5Core2.h>
#include "Adafruit_MAX31855.h"

#define MAXDO   14
#define MAXCS   19
#define MAXCLK  13

float bat_v = 0.0;
float bat_p = 0.0;
int bat_changed_flg = 0;
float temp = 0.0;
int temp_changed_flg = 0;

int button_pushed_flg_start = 0;
int button_pushed_flg_stop = 0;
int button_pushed_flg_reset = 0;

int curr_msec_from_start = 0;
int stop_msec = 0;

int flg_start = 0;

static const int space = 10;
// LCD SIZE: 320 x 240
static const int chart_w = 320 - (16 + space);
static const int chart_h = 240 - (26 + 16 + space + space);

// 20min=1200sec
// 100msec更新
// 1200x100=120K
#define MAX_TIME_MINUTES 20
#define INTERVAL_MSEC 100
#define MAX_TIME_MSEC MAX_TIME_MINUTES * 60 * 1000  // 1200k
#define MAX_TEMP 300
int temps_target[chart_w] = {};
int temps_measured[chart_w] = {};
float bin_x = float(MAX_TIME_MSEC) / float(chart_w);  // 1200k / 294 = 4081.6 4kmsecで1pix=4secで1pix
float bin_y = float(MAX_TEMP) / float(chart_h);  // 300 / 240 = 1.25

int target[MAX_TIME_MINUTES] = {};
int haze[MAX_TIME_MINUTES] = {};

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void get_bat_status() {
  bat_v = M5.Axp.GetBatVoltage();
  float _bat_p = ( bat_v < 3.2 ) ? 0 : ( bat_v - 3.2 ) * 100;
  if (bat_p != _bat_p) {
    bat_p = _bat_p;
    bat_changed_flg = 1;
  }
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
    float _temp = float(c);
    if (_temp != temp) {
      temp = _temp;
      temp_changed_flg = 1;
    }
    Serial.print("temp = ");
    Serial.print(temp);
    Serial.println(" C");
  }
  return temp;
}

void check_touch_button() {
  // left touch
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.y > 240 && pos.x < 109) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);
    button_pushed_flg_start = 1;
  }
  // center touch
  if (pos.y > 240 && 109 < pos.x && pos.x < 218) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);
    button_pushed_flg_stop = 1;
  }
  // right touch
  if (pos.y > 240 && pos.x > 218) {
    M5.Axp.SetLDOEnable(3, true);  // Open the vibration.
    delay(100);
    M5.Axp.SetLDOEnable(3, false);
    button_pushed_flg_reset = 1;
  }
}

void show_lcd_bottom() {
  M5.Lcd.setTextDatum(7);  // 中央下基準へ
  M5.Lcd.drawCentreString("Start", (M5.Lcd.width()/3)/2, M5.Lcd.height(), 4);  // font4=26px
  M5.Lcd.drawCentreString("Stop", M5.Lcd.width()/2, M5.Lcd.height(), 4);
  M5.Lcd.drawCentreString("Reset", M5.Lcd.width()-((M5.Lcd.width()/3)/2), M5.Lcd.height(), 4);
  M5.Lcd.setTextDatum(0);  // 左上基準に戻す
}

void show_lcd_top() {
  // bat
  if (bat_changed_flg == 1) {
    M5.Lcd.fillRect(M5.Lcd.width()/2, 0, M5.Lcd.width()/2, 16, BLACK);
    M5.Lcd.setTextDatum(2);  // 右上基準へ
    M5.Lcd.drawRightString("bat: " + String(bat_v) + "(V) " + String(bat_p) + "(%)", M5.Lcd.width(), 0, 2);  // font2=16pix
    M5.Lcd.setTextDatum(0);  // 左上基準に戻す
    bat_changed_flg = 0;
  }
  // temp
  if (temp_changed_flg == 1) {
    M5.Lcd.fillRect(0, 0, M5.Lcd.width()/2, 16, BLACK);
    M5.Lcd.drawString("Temp: " + String(temp) + "(C)", 0, 0, 2);  // font2=16pix
    temp_changed_flg = 0;
  }

}
void show_lcd_init_chart() {
  // background
  M5.Lcd.fillRect(
    space,
    16 + space,
    M5.Lcd.width() - (16 + space),
    M5.Lcd.height() - (26 + 16 + space + space),
    M5.Lcd.color565(50, 50, 50)  // rgb
  );
  // grid
  int interval_h = 10; // 縦10段階
  int str_interval_h = 3;  // 3メモリごと
  int temp_interval = int(float(MAX_TEMP) / float(interval_h));  // 10段階
  int pix_interval_h = int(float(chart_h) / float(interval_h));  // 10段階
  
  int interval_w = MAX_TIME_MINUTES;  // 横"分"段階
  int str_interval_w = 5;  // 5メモリごと
  int minutes_interval = int(float(MAX_TIME_MINUTES) / float(interval_w));
  int pix_interval_w = int(float(chart_w) / float(interval_w));
  int memori_no = 0;
  for (int h = 0; h < chart_h; h = h + pix_interval_h) {
    int draw_h = M5.Lcd.height() - (h + 26 + space);
    M5.Lcd.drawLine(space, draw_h, M5.Lcd.width()-(2 * space), draw_h, M5.Lcd.color565(100, 100, 100));
    if (memori_no % str_interval_h == 0) {
      if (memori_no == 0) {
        M5.Lcd.setTextDatum(4);  // 中心基準へ
      } else {
        M5.Lcd.setTextDatum(3);  // 左中基準へ
      }
      M5.Lcd.drawString(String(temp_interval * memori_no), space, draw_h, 2);  // font2=16pix
    }
    memori_no = memori_no + 1;
  }
  memori_no = 0;
  for (int w = 0; w < chart_w; w = w + pix_interval_w) {
    int draw_w = w + space;
    M5.Lcd.drawLine(draw_w, 16 + space, draw_w, M5.Lcd.height()- (26 + space), M5.Lcd.color565(100, 100, 100));
    if (memori_no % str_interval_w == 0) {
      if (memori_no != 0) {
        M5.Lcd.setTextDatum(7);  // 下中基準へ
        M5.Lcd.drawString(String(minutes_interval * memori_no), draw_w, M5.Lcd.height()- (26 + space), 2);  // font2=16pix
      }
    }
    memori_no = memori_no + 1;
  }
  M5.Lcd.setTextDatum(0);  // 左上基準に戻す

  // target
  target[0] = 75;
  target[1] = 100;
  target[2] = 110;
  target[3] = 130;
  target[4] = 150;
  target[5] = 160;
  target[6] = 175;
  target[7] = 180;
  target[8] = 190;//1
  target[9] = 200;//1
  target[10] = 210;
  target[11] = 220;//2
  target[12] = 250;//2
  target[13] = 250;
  haze[0] = 0;
  haze[1] = 0;
  haze[2] = 0;
  haze[3] = 0;
  haze[4] = 0;
  haze[5] = 0;
  haze[6] = 0;
  haze[7] = 0;
  haze[8] = 1;//1
  haze[9] = 1;//1
  haze[10] = 0;
  haze[11] = 2;//2
  haze[12] = 2;//2
  haze[13] = 0;
  // int _curr_msec_from_start = 0;
  int draw_x_prev = 0;
  int draw_y_prev = 0;
  for (int i = 0; i < MAX_TIME_MINUTES; i++) {
    if (target[i] == 0) {
      break;
    }
    int target_tmp = target[i];
    int target_msec = i * 60 * 1000;
    int x = int(float(target_msec) / bin_x);
    int y = int(float(target_tmp) / bin_y);
    int draw_x = x + space;
    int draw_y = M5.Lcd.height() - (y + 26);
    if (draw_x_prev > 0 && draw_y_prev > 0) {
      if (haze[i-1] == 1) {
        // 1ハゼ
        M5.Lcd.drawLine(draw_x_prev, draw_y_prev, draw_x, draw_y, MAGENTA);
      } else if (haze[i-1] == 2) {
        // 2ハゼ
        M5.Lcd.drawLine(draw_x_prev, draw_y_prev, draw_x, draw_y, MAROON);
      } else {
        // 通常
        M5.Lcd.drawLine(draw_x_prev, draw_y_prev, draw_x, draw_y, YELLOW);
      }
    }
    draw_x_prev = draw_x;
    draw_y_prev = draw_y;
  }
}

void setup() {
  // M5
  M5.begin();
  delay(500);
  init_lcd();
  M5.Spk.begin();

  // serial
  Serial.begin(9600);
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  Serial.println("setup() DONE.");
  show_lcd_init_chart();
}

void loop() {
  get_bat_status();
  get_temp();
  check_touch_button();
  show_lcd_bottom();
  show_lcd_top();
  if (button_pushed_flg_start == 1) {
    // start is pushed.
    button_pushed_flg_start = 0;
    curr_msec_from_start = 0;
    if (stop_msec != 0) {
      curr_msec_from_start = stop_msec;
    }
    stop_msec = 0;
    flg_start = 1;
  }

  if (flg_start == 1) {
    int x = int(float(curr_msec_from_start) / bin_x);
    if (x >= chart_w) {
      x = chart_w - 1;
    }
    int y = int(temp / bin_y);
    if (y >= chart_h) {
      y = chart_h - 1;
    }
    temps_measured[x] = y;
    for (int i = 0; i < chart_w; i++) {
      if (temps_measured[i] > 0) {
        int r = 1;
        int draw_x = i + space;
        int draw_y = M5.Lcd.height() - (temps_measured[i] + 26);
        M5.Lcd.fillCircle(draw_x, draw_y, r, RED);  // 円塗り潰し
      }
    }
    curr_msec_from_start = curr_msec_from_start + INTERVAL_MSEC;
  }

  if (button_pushed_flg_stop == 1) {
    // stop is pushed.
    button_pushed_flg_stop = 0;
    flg_start = 0;
    stop_msec = curr_msec_from_start;
  }

  if (button_pushed_flg_reset == 1) {
    // reset is pushed.
    button_pushed_flg_reset = 0;
    curr_msec_from_start = 0;
    stop_msec = 0;
    for (int i = 0; i < chart_w; i++) {
      temps_measured[i] = 0;
    }
    show_lcd_init_chart();
  }
  delay(INTERVAL_MSEC);
}


