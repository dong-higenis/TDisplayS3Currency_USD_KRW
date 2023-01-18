#include <TFT_eSPI.h>
#include "time.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector> 
#include <HTTPClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson.git

#include "NotoSansBold36.h"
#include "Latin_Hiragana_24.h"
#include "NotoSansMonoSCB20.h"
#include "Final_Frontier_28.h"
#include "NotoSansBold15.h"
#include "curr.h"
TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite sprite= TFT_eSprite(&tft);

const char* ntpServer = "pool.ntp.org";

// 현재 시간을 한국 시간(GMT +9) 으로 변경 (3600 * 9)
const long  gmtOffset_sec = 32400; //time zone * 3600 , my time zone is  +1 GTM
const int   daylightOffset_sec = 32400;

#define small NotoSansBold15
#define large NotoSansBold36
#define latin Latin_Hiragana_24
#define midle NotoSansMonoSCB20
#define final Final_Frontier_28

#define c1 0xCE79  //colour gray
#define c2 0xAD01 //color yellow
#define c3  0x80C3     //red

int px=10;
int py=10;

const char* ssid     = "*********";       ///EDIIIT
const char* password = "*********";
String date2="";
String payload="";
StaticJsonDocument<6000> doc;
String endpoint2 ="https://cdn.jsdelivr.net/gh/fawazahmed0/currency-api@1/latest/currencies/hrk.min.json";

enum {
  CURRENCY_EUR = 0, // 0: Euro
  CURRENCY_HRK,     // 1: Croatian Kuna
  CURRENCY_USD,     // 2: US Dollar
  CURRENCY_GBP,     // 3: Pound Sterling
  CURRENCY_AUD,     // 4: Australian Dollar
  CURRENCY_CAD,     // 5: Canadian Dollar
  CURRENCY_CHF,     // 6: Swiss Franc
  CURRENCY_JPY,     // 7: Japanese Yen
  CURRENCY_CNY,     // 8: Chinese Renminbi
  CURRENCY_NZD,     // 9: New Zealand Dollar
  CURRENCY_BTC,     // 10: BitCoin
  CURRENCY_KRW,     // 11: Korean Won
  MAX_CURRENCY,     // 12
};

// KRW 추가
String cur[MAX_CURRENCY]={"eur","hrk","usd","gbp","aud","cad","chf","jpy","cny","nzd","btc","krw"};

// USD 에서
int fromCurrency=CURRENCY_USD;
// KRW로 변환
int toCurrency=CURRENCY_KRW;

double amount=1;

// WON 문구 추가
String big[MAX_CURRENCY]={"EUR","HRK","USD","GBP","AUD","CAD","CHF","JPY","CNY","NZD","BTC","WON"};
// Korean Won 문구 추가
String curNames[MAX_CURRENCY]={"Euro","Croatian Kuna","US Dollar","Pound Sterling","Australian Dollar","Canadian Dollar","Swiss Franc","Japanese Yen","Chinese Renminbi","New Zealand Dollar","BitCoin","Korean Won"};

float res;
String ip="";

int deb1=0; //button debounce
int deb2=0; //button debounce

char timeHour[3];
char timeMin[3];
char timeSec[3];

long period=1000;
long currentTime=0;

void setup() {

pinMode(0,INPUT_PULLUP);  
pinMode(14,INPUT_PULLUP);  
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  Serial.begin(115200);
  Serial.printf("Connection... \nssid %s, password %s\n", ssid, password);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(400);
  Serial.print(".");
 }
  ip=WiFi.localIP().toString();
  Serial.printf("\nConnection success ip %s\n", ip);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  sprite.createSprite(320,170);
  sprite.setTextColor(TFT_GREEN,TFT_BLACK);
  sprite.loadFont(midle);
  sprite.setSwapBytes(true);
  getData();
  printLocalTime();
  draw();
}

void getData()
{
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
   HTTPClient http;
   String    endpoint="https://cdn.jsdelivr.net/gh/fawazahmed0/currency-api@1/latest/currencies/"+cur[fromCurrency]+".min.json";
   http.begin(endpoint); //Specify the URL
   int httpCode = http.GET();  //Make the request
   if (httpCode > 0) { //Check for the returning code
   payload = http.getString();
   char inp[payload.length()];
   payload.toCharArray(inp,payload.length());
   deserializeJson(doc,inp);

    date2=String(doc["date"].as<const char*>());
    res=doc[cur[fromCurrency]][cur[toCurrency]].as<float>();    
   }}
  
}

void draw()
{

 
sprite.pushImage(0,0,320,170,curr);

sprite.loadFont(midle);
sprite.setTextColor(c1,TFT_BLACK);
sprite.drawString(date2,185,19);
sprite.fillSmoothRoundRect(276, 107, 30, 24, 4, c3,TFT_BLACK);
sprite.drawString(String(timeSec),280,110);
sprite.unloadFont();

sprite.loadFont(final);
// WON 문자열 잘리는 문제로 " - " 에서 "-"로 줄임
sprite.drawString(big[fromCurrency]+"-"+big[toCurrency],12,60);
sprite.unloadFont();

sprite.setTextColor(c2,TFT_BLACK);
sprite.loadFont(large);

// 두번째 자리 2: 소숫점 자릿수 최대 2자리 표현
sprite.drawFloat(res,2,12,88);
sprite.unloadFont();
sprite.setTextColor(c1,TFT_BLACK);

sprite.loadFont(small);
sprite.drawString("IP:"+ip,170,146);
sprite.drawString(curNames[fromCurrency]+" to ",12,126);
sprite.drawString(curNames[toCurrency],12,144);
sprite.setTextColor(c2,TFT_BLACK);
sprite.drawString("TIME",232,107);
sprite.setTextColor(c1,TFT_BLACK);
 
sprite.unloadFont();
sprite.drawString(String(timeHour)+":"+String(timeMin),172,52,7);

sprite.pushSprite(0,0);
}



void printLocalTime()
  {
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
   return;}
  
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMin,3, "%M", &timeinfo);
  strftime(timeSec,3, "%S", &timeinfo);
  }

void loop() {
if(digitalRead(0)==0)
{
  if(deb1==0)
  {
    deb1=1;
    fromCurrency++;
    if(fromCurrency==11) fromCurrency=0;
    getData();
    draw();
  }  
}else {deb1=0;}

if(digitalRead(14)==0)
{
  if(deb2==0)
  {
    deb2=1;
    toCurrency++;
    if(toCurrency==11) toCurrency=0;
    getData();
    draw();
  }  
}else {deb2=0;}

if(millis()>currentTime+period)
{
  printLocalTime();
  draw();
  currentTime=millis();
  
}

}
