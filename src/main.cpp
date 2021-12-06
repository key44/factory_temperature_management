#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "UNIT_ENV.h"
#include <time.h>

//-------------------------設定------------------------------//
const char* ssid = "SSID";//wifiのSSID
const char* password = "PASSWORD";//wifiのパスワード
String locat = "工場１";//計測場所名
int interval = 60000;//データの更新間隔（ms）
//----------------------------------------------------------//
const char* url = "http://168.138.52.183:9200/temp/_doc/";

#define JST 3600* 0
String json_text ="";
SHT3X sht30;
QMP6988 qmp6988;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;


void setup() {
  Serial.begin(115200);

  M5.begin(); //Init M5Stack.  初始化M5Stack
  M5.Lcd.setRotation(3);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

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
  strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&t));
  Serial.println(date);
  json_text ="{\"location\":\"" + locat + "\",\"date\": \"" + date + "\",\"temp\": " + tmp + ",\"humid\": "+ hum/100 + "}";


  Serial.print(json_text);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(url);  //curlコマンドと同じURi『http://192.168.0.100/datastore/ext/obank/2/ch/0/name 』
    http.addHeader("Content-Type", "application/json");
    //http.PUT("{\"mappings\":{\"properties\":{\"location\":{\"type\":\"keyword\"},\"date\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss||yyyy-MM-dd||epoch_millis\"},\"temp\":{\"type\":\"float\"},\"humid\":{\"type\":\"float\"}}}}");
    int httpResponseCode = http.POST(json_text);  //curlコマンドと同じ内容のJSON『{\"value\":\"My favorite channel\"} 』
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
  M5.lcd.fillRect(0,20,100,60,BLACK); //Fill the screen with black (to clear the screen).  将屏幕填充黑色(用来清屏)
  M5.lcd.setCursor(0,20);
  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPress:%2.0fhPa\r\n", tmp, hum, pressure/100);

  delay(interval);
}