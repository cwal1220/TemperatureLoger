#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "ParkChan";
const char* password = "88888888";
const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbxkHLiS1W8ntbogJ-6_Fz_w9jah1jTWIEOJgsdmIDe57Pcu9s8Yc479aRiH1JUGYfUX3w"; 

void setup() {
  Serial.begin(115200);

  //Wifi 연결하기
  connectWiFi();
}

void loop() {
  float t = 25.0;

  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.println("------------");
  
  sendData(t); //데이터 전송함수 실행
  delay(10000); //10초 원하는 간격을 줌
}

void sendData(float temp){
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClientSecure client;
  client.setInsecure();//[중요]이것을 안해 주면 연결 자체가 안된다.
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }


  //스트링으로 바꿈
  String string_temp     =  String(temp);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temp;
  Serial.print("requesting URL: ");
  Serial.println(url);
  //전송
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "User-Agent: BuildFailureDetectorESP8266\r\n" +
    "Connection: close\r\n\r\n");
  Serial.println("request sent");
}

void connectWiFi(){
  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.hostname("Name");
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
