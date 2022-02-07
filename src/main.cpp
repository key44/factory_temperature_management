#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "UNIT_ENV.h"
#include <time.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

//-------------------------設定------------------------------//
String locat = "生産技術";//計測場所名
int interval = 30000;//データの更新間隔（ms）
const char* url = "https://hogehoge";//json受け取りサーバー

char * wifi_setup[] = {
  //"SSID","password",
  "SSID","password"
};
//----------------------------------------------------------//

int wifi_no = sizeof(wifi_setup)/sizeof(wifi_setup[0]) -1;
#define JST 3600* 0
String json_text ="";
SHT3X sht30;
QMP6988 qmp6988;
WiFiMulti wifiMulti;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;


void setup() {
  Serial.begin(115200);

  M5.begin(); //Init M5Stack.  初始化M5Stack
  M5.Lcd.setRotation(3);
  for  (int i = 0; i < wifi_no; i= i+2) {
    wifiMulti.addAP(wifi_setup[i], wifi_setup[i+1]);
  }
  delay(200);

  if(wifiMulti.run() == WL_CONNECTED) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }

  Serial.println("Connecting");
  Serial.println(WiFi.localIP());

  M5.lcd.setTextSize(2);
  Wire.begin();
  qmp6988.init();
  M5.lcd.println(F("ENV Unit III test"));
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

}

void loop() {
  pressure = qmp6988.calcPressure();
  if(sht30.get()==0){ //Obtain the data of shT30.  获取sht30的数据
    tmp = sht30.cTemp;  //Store the temperature obtained from shT30.  将sht30获取到的温度存储
    hum = sht30.humidity; //Store the humidity obtained from the SHT30.  将sht30获取到的湿度存储
  }else{
    tmp=0,hum=0;
  }
  time_t t;
  t = time(NULL);
  char date[64];
  StaticJsonDocument<128> doc;//Json形式を定義
  char pubMessage[256];
  strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&t));

  //JSONメッセージの作成
  doc["location"] = locat;
  doc["timestamp"] = date;
  doc["temp"] = tmp;
  doc["humid"] = hum/100;
  doc["press"] = pressure/100;
  serializeJson(doc, pubMessage);

  Serial.print(pubMessage);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(url);  
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(pubMessage);
    if (httpResponseCode > 0) {

      String response = http.getString();

      Serial.println(httpResponseCode);
      Serial.println(response);

    } else {

      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);

    }

    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }
  M5.lcd.fillRect(0,20,100,60,BLACK); 
  M5.lcd.setCursor(0,20);
  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPress:%2.0fhPa\r\n", tmp, hum, pressure/100);

  delay(interval);

}