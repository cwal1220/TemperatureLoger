#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <EEPROM.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* host = "script.google.com";
String sheetID = "AKfycbxkHLiS1W8ntbogJ-6_Fz_w9jah1jTWIEOJgsdmIDe57Pcu9s8Yc479aRiH1JUGYfUX3w"; 
const int sendDelayUs = 15 * 60 * 1000 * 1000; // 15 minutes. WiFi 연결 시 5초 추가 소요됨.

char eRead[30];
char ssid[30];
char password[30];
char deviceid[30];
byte len;
bool captive = true;

IPAddress apIP(192, 168, 1, 1);
ESP8266WebServer webServer(80);

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS D2
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

String responseHTML = ""
    "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Wi-Fi Setting Page</title></head><body><center>"
    "<p>Wi-Fi Setting Page</p>"
    "<form action='/button'>"
    "<p><input type='text' name='ssid' placeholder='SSID' onblur='this.value=removeSpaces(this.value);'></p>"
    "<p><input type='text' name='password' placeholder='WLAN Password'></p>"
    "<p><input type='text' name='deviceid' placeholder='Device ID'></p>"
    "<p><input type='submit' value='Submit'></p></form>"
    "<p>Wi-Fi Setting Page</p></center></body>"
    "<script>function removeSpaces(string) {"
    "   return string.split(' ').join('');"
    "}</script></html>";

void setup() 
{
  Serial.begin(115200);
  EEPROM.begin(1024);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, initDevice, FALLING);
  ReadString(0, 30);
  if (!strcmp(eRead, ""))
  {
    setup_captive();
  }
  else 
  {
    captive = false;
    strcpy(ssid, eRead);
    ReadString(30, 30);
    strcpy(password, eRead);
    ReadString(60, 30);
    strcpy(deviceid, eRead);
    setup_runtime();  
  }
}

void loop() 
{
  if (captive)
  {
    webServer.handleClient();
  }
  else
  {
    float t = getTemperature();
    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.println("------------");
    // Send Temperature
    sendData(t);
    // Ussing Deepsleep mode. Wire D0 pin with RST pin
    ESP.deepSleep(sendDelayUs);
//     delay(sendDelayUs / 1000);
  }
}

void setup_runtime() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
    yield;
    if(i++ > 20) 
    {
      ESP.restart();
      return; 
    }
  }
  Serial.println("");
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  Serial.println("HTTP server started");  
}

void setup_captive() 
{    
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("ESP_LOGGER");
  
  webServer.on("/button", button);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  Serial.println("Captive Portal Started");
}

void button()
{
  Serial.println("button pressed");
  Serial.println(webServer.arg("ssid"));
  SaveString(0, (webServer.arg("ssid")).c_str());
  SaveString(30, (webServer.arg("password")).c_str());
  SaveString(60, (webServer.arg("deviceid")).c_str());
  webServer.send(200, "text/plain", "OK");
  ESP.restart();
}

void SaveString(int startAt, const char* id) 
{ 
  for (byte i = 0; i <= strlen(id); i++)
  {
    EEPROM.write(i + startAt, (uint8_t) id[i]);
  }
  EEPROM.commit();
}

void ReadString(byte startAt, byte bufor) 
{
  for (byte i = 0; i <= bufor; i++) 
  {
    eRead[i] = (char)EEPROM.read(i + startAt);
  }
  len = bufor;
}

void handleNotFound()
{
  String message = "File Not Found\n";
  webServer.send(404, "text/plain", message);
}

ICACHE_RAM_ATTR void initDevice() 
{
  Serial.println("Flushing EEPROM....");
  SaveString(0, "");
  ESP.restart();
}

void sendData(float temp)
{
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClientSecure client;
  // connection insecure, must be used
  client.setInsecure();
  if (!client.connect(host, 443)) 
  {
    Serial.println("connection failed");
    return;
  }
  // Float to String
  String string_temp = String(temp);
  
  String url = "/macros/s/" + sheetID + "/exec?temperature=" + string_temp + "&deviceid=" + deviceid;
  Serial.print("requesting URL: ");
  Serial.println(url);
  // Send HTTP request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "User-Agent: BuildFailureDetectorESP8266\r\n" +
    "Connection: close\r\n\r\n");
  Serial.println("request sent");
}

float getTemperature()
{
  float ret;
  sensors.requestTemperatures();
  ret = sensors.getTempCByIndex(0);
  return ret;
}
