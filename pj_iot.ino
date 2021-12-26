#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <RFID.h>
#include <TridentTD_LineNotify.h>
#define LINE_TOKEN "bzcTOzbIxFrfDSfNfDWSCfFWEbmMC8xVdVsNJKupOkb"
#define ON_Board_LED 2  
#define SS_PIN D2
#define RST_PIN D1

RFID rfid(SS_PIN, RST_PIN);

const char* ssid = "spedy"; //--> ชื่อ WIFI
const char* password = "spedy123"; //-->  รหัสผ่าน WIFI 
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client; 
String GAS_ID = "AKfycbxFUxA_HyozBflnLpQiFE14x5yrXoT--CHVEFDaWIxBbyUjsP-yPSeeECxpFX-2Nm82Ew"; //test3
int buz = D8; 
int i;
int stat[3]={0,0,0};
char list[3][50]={"Phattarapol","Phakawan","XXXXX"};
int led_out = D0; 
int led_in = D3; 
String n ;
String s ;

void setup() {
  Serial.begin(115200); 
  delay(500);
  WiFi.begin(ssid, password); 
  Serial.println("");  
  pinMode(led_in, OUTPUT);
  pinMode(led_out, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, LOW);
    digitalWrite(led_in,0);
    digitalWrite(led_out,0);
    delay(250);
    digitalWrite(led_in,1);
    digitalWrite(led_out,1);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); 
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  client.setInsecure();
  SPI.begin();
  rfid.init();
  digitalWrite(led_in,0);
  digitalWrite(led_out,0);
}

void loop() {
  int check = 1;
  Serial.println("Please Scan");
  if (rfid.isCard()) {
    LINE.setToken(LINE_TOKEN);
    while(check==1){
    if (rfid.readCardSerial()) {
        Serial.println("Cardnumber:");
        Serial.print("Dec: ");
        Serial.print(rfid.serNum[0], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[1], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[2], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[3], DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[4], DEC);
        Serial.println(" .......................");
      if(rfid.serNum[0]==131 && rfid.serNum[1]==138 && rfid.serNum[2]==170 && rfid.serNum[3]==24 && rfid.serNum[4]==187) {
        i = 0; 
        buz_i();     
        if(stat[i]== 0){
          stat[i]= 1;
           n = list[i];
           s = "IN";
          sendData(n,s); 
          }else{
            stat[i]= 0;
             n = list[i];
             s = "OUT";
            sendData(n,s);       
            }       
      }else if(rfid.serNum[0]==32 && rfid.serNum[1]==160 && rfid.serNum[2]==65 && rfid.serNum[3]==137 && rfid.serNum[4]==72){
        i = 1; 
        buz_i();     
        if(stat[i]== 0){
          stat[i]= 1;
           n = list[i];
           s = "IN";
          sendData(n,s); 
          }else{
            stat[i]= 0;
             n = list[i];
             s = "OUT";
            sendData(n,s);       
           } 
        }else{
        buz_o();
        Serial.println("No data");
        }
    }
    check = 0;
  }
  rfid.halt();
  
}
delay(250);     
}

void sendData(String value,String value2) { 
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  String string1 = value; 
  String string2 = value2;
  String url = "/macros/s/" + GAS_ID + "/exec?name=" + string1 + "&stat=" + string2; //  4 variables 
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
} 
void buz_i(){
  digitalWrite(led_in,1);
  digitalWrite(buz,1);
  delay(150);
  digitalWrite(buz,0);
  delay(50);
  digitalWrite(buz,1);
  delay(150);
  digitalWrite(buz,0);
  delay(50);
   digitalWrite(buz,1);
  delay(850);
  digitalWrite(buz,0);
  digitalWrite(led_in,0);
  }
void buz_o(){
  digitalWrite(led_out,1);
  digitalWrite(buz,1);
  delay(1000);
  digitalWrite(buz,0);
  digitalWrite(led_out,0);
}
