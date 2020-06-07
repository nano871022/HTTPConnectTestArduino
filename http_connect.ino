#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#define WAITING_LOOP 30000
#define TIME_LED_ON 500
#define WAITING_1_SEG 1000
#define WAITING_1_2_SEG 500
#define WAITING_TIME_OUT_CONNECT 15000

int waiting_loop = WAITING_LOOP;
const char *ssid = "TIGO-B13F";
const char *password = "2NB112101391";
const char *host = "docs.google.com";
const int httpsport = 443;
//const char fingerprint[] PROGMEM = "95 E2 82 36 E0 41 A6 FA 8E 53 8C 18 85 F6 F3 B2 2D C7 A2 C9";//www.google.com
const char fingerprint[] PROGMEM = "1C 9A D3 68 40 3A 92 46 DC D1 51 A2 71 CF 71 67 5A AC F5 5B";//docs.google.com
String statusConnect;
String descripcionConnect;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);
  delay(WAITING_1_SEG);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("");
  Serial.print("Connection");
  while(WiFi.status() != WL_CONNECTED){
    blindLed(TIME_LED_ON);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IPMachine ");
  Serial.println(WiFi.localIP());
}

void loop() {
  sendData();
  blindLed(TIME_LED_ON);
  delay(waiting_loop);
}

void sendData(){
  WiFiClientSecure httpsClient;
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(WAITING_TIME_OUT_CONNECT);
  delay(WAITING_1_SEG);
  Serial.print("Try connecting");
  int retryConnect = 0;
  while((!httpsClient.connect(host,httpsport)) && (retryConnect < 30)){
     blindLed(100);
     Serial.print(".");
     retryConnect++;
  }
  Serial.println("");
  Serial.print("Numero de reintentos usados: ");
  Serial.println(retryConnect);
  
  if(retryConnect == 30){
    Serial.println("Connection Failed");
    waiting_loop = waiting_loop + WAITING_LOOP;
    Serial.print("Next try ");
    Serial.print(waiting_loop/1000);
    Serial.println("seg.");
    return;
  }else{
    Serial.println("Connection Succesfull");
    waiting_loop =  + WAITING_LOOP;
  }

  httpsClient.print(String("GET /forms/d/e/1FAIpQLSdal5EO0E42S8dvgnGJzjhsj88afyhHqbBa9fTRU5FXZh-5Qw/formResponse?entry.802876412=0&entry.1605107394=0&entry.1627871440=isOpen&entry.428488488=0")+" HTTP/1.1\r\n"+
                    "Host: "+ host +"\r\n"+
                    "User-Agent: Arduino/nodeMCU - ESP8266"+
                    "Accept: text/html,application/xhtml+xml"+
                    "Accept-Language: es-CO"+
                    "Cache-Control: max-age=0"+
                    "Connection: close \r\n\r\n");
  int empty = 0;
  Serial.print("Reading headers");
  while(httpsClient.connected()){
    String line = httpsClient.readStringUntil('\n');
    if(line == "\r" || (line == "\n" && line.length() == 1)){
      Serial.println("");
      Serial.println("headers received");
      break;
    }if(line.length() < 2) {
      Serial.print(".");
      if(empty == 10){
        break;
      }
      empty++;
    }else{
      statusConnection(line);
      Serial.println("");
      Serial.print(line);
    }
    blindLed(100);
  }
  String line;
  Serial.println("");
  Serial.println("====================================================");
  Serial.println("Read response");
  Serial.print("Estado Conneccion ");
  Serial.println(statusConnect);
  Serial.print("Descripcion Conneccion ");
  Serial.println(descripcionConnect);
  while(httpsClient.available()){
    line = httpsClient.readStringUntil('\n');
    Serial.println(line);
    blindLed(100);
  }
  Serial.println("End connection");    
}

void statusConnection(String line){
      if(line.indexOf("HTTP")>-1){
        int spaceDetected = line.indexOf(" ");
        statusConnect = line.substring(spaceDetected+1);
        spaceDetected = statusConnect.indexOf(" ");
        descripcionConnect = statusConnect.substring(spaceDetected+1);
        statusConnect = statusConnect.substring(0,spaceDetected);
      }
}

void blindLed(int timer){
  digitalWrite(LED_BUILTIN,HIGH);
  delay(timer);
  digitalWrite(LED_BUILTIN,LOW);  
  digitalWrite(LED_BUILTIN,LOW);  
}
