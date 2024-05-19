//これは、ArduinoのSPIを使ってK型熱電対用センサーIC、MAX6675を
//評価するためのサンプルSketchです。
//
//SPIを使うので、SPI用ヘッダーファイルをインクルード
#include <SPI.h>
//MAX6675のデータ格納用変数
#define MYSS 19
int rdata;

//MAX6675用SPIの各種設定
//SPIクロックは4MHz、データの出力順は、MSBが先、
//SPIモードは0（クロックはローレベルからスタート、立ち上がりエッジでデータラッチ)
SPISettings MAX6675Setting (4000000, MSBFIRST, SPI_MODE0);

void setup() {
//SPIを開始する
SPI.begin();
//COMポートを有効化
Serial.begin(9600);
//MAX6675は、電源投入時にADCの変換時間である最大220msを待たないと、
//内部ADCのデータが不安定になるようで、念のため待ち時間をもうける
//決め打ちで500ms
delay(500);
}

void loop() {
//SPIの設定を指定し、SPIを開始する
SPI.beginTransaction(MAX6675Setting);

//SSピン(D10)をローレベルとし、MAX6675を選択
digitalWrite(MYSS, LOW);
//MAX6675からの出力データを読み込む
rdata=SPI.transfer(0xFF) << 8;
rdata=rdata+SPI.transfer(0xFF);
//SSピンをハイレベルとし、MAX6675の選択解除
digitalWrite(MYSS, HIGH);

//SPIを終了する。
SPI.endTransaction();

//MAX6675が受け取った16ビットの生データをCOMに表示する
//オシロスコープを使ったデータデコードの確認用にHEXデータをCOMに表示
Serial.print(rdata, HEX);
Serial.print(", ");
//MAX6675が受け取ったデータのうち、3ビット右シフトして12ビットの温度データのみ抜き出し、
//MAX6675の1LSB分の温度分解能である0.25℃をかけて、温度を計算しCOMに表示する
Serial.print((rdata>>3) * 0.25);
Serial.println(" deg C");

delay(1000);

}
