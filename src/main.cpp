#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include "httpclient.h"
#define DEBUG
const char* ssid     = "STTekno_lt2";
const char* password = "STTekno12345";
int i = 0;
int count  = 0;
int value = 0;
SoftwareSerial mySerial (5, 16);
unsigned char incomingByte;
String userID = "60f50845aefb7b6b73a1f636";
String myId[7];
String cok;
String token;
String mode="1";
String berat = "0";
SocketIoClient Socket;
int period = 10000;
unsigned long time_now = 0;
int second = 100;
unsigned long time_now1 = 0;
StaticJsonDocument<200> doc;
int motorDC = 4;
#define echoPin 12 //Echo Pin
#define trigPin 14 //Trigger Pin

// void event(const char * payload, size_t length) {
//   Serial.print("ini change Mode : ");
//   Serial.println(payload);
//   mode = String(payload);
// }
long duration, distance;
void event1(const char * payload, size_t length) {
  Serial.print("ini response socket : ");
  Serial.println(payload);
  Serial.println(mode);
  if(mode == "1"){
    mode = "0";
  }else{
    mode = "1";
  }
  String Data = "{\"id_User\":\"60f50845aefb7b6b73a1f636\",\"mode\":\""+ mode
                  +"\"}";
  Socket.emit("MyStatus",Data.c_str());
}

void fromTimbangan(const char * payload, size_t length) {
  Serial.print("ini response berat : ");
  Serial.println(payload);
  berat = String(payload);
}

void setup (){
  Serial.begin (9600);
  mySerial.begin (9600);
  pinMode(motorDC,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  Serial.println ("begin initial Serial!\n");
  
  String payload = httpPostToENDpoint("login",
  "{\"email\":\"fakhrilak23@gmail.com\",\"password\":\"fakhrilak\"}","x");
  Serial.println(payload);
  deserializeJson(doc, payload);
  String tkn = doc["token"];
  token = tkn;

  Socket.begin("192.168.10.111",3004); 
  Serial.println("connect?");
  // Socket.on("changeStatus60f50845aefb7b6b73a1f636",event);
  Socket.on("giveStatus60f50845aefb7b6b73a1f636",event1);
  Socket.on("toTagging60f50845aefb7b6b73a1f636",fromTimbangan);
}

void loop (){
  String mystring = "";
  if(mode == "1"){
      if(count <= 8){
        while (mySerial.available () > 0 && count != 100){
            String a;
            
            if(count <= 7){
              incomingByte = mySerial.read ();
              Serial.print("baca = ");
              Serial.println(count);
              // Serial.print(String(incomingByte,HEX));
              a = String(incomingByte,HEX);
              myId[count] = a;
              count = count + 1;
            }else{
              cok ="";
              for(int s = 0; s <= 7 ; s++){
              cok = cok+myId[s];
              }
              String res_post = httpPostToENDpoint("id-tag",
              "{\"id_tag\":\""+ cok +"\"}",token);
              deserializeJson(doc, res_post);
              String message = doc["message"];
              String Dataaa = "{\"User_id\":\"60f50845aefb7b6b73a1f636\",\"message\":\""+ message
                      +"\"}";
              Socket.emit("ResTimbangan",Dataaa.c_str());
              delay(1000);
              cok = "";
              count = 0;
            }
        }
      }
  }else if(mode == "0"){
      if(count <= 8){
        while (mySerial.available () > 0 && count != 100){
            String a;
            
            if(count <= 7){
              incomingByte = mySerial.read ();
              Serial.print("baca = ");
              Serial.println(count);
              // Serial.print(String(incomingByte,HEX));
              a = String(incomingByte,HEX);
              myId[count] = a;
              count = count + 1;
            }else{
              cok ="";
              for(int s = 0; s <= 7 ; s++){
              cok = cok+myId[s];
              }
               String id_timb = "1";
               String res_post = httpPostToENDpoint("weight-got",
                "{\"id_timbangan\":\""+  id_timb 
                    +"\",\"id_tag\":\""+ cok
                    +"\",\"bb\":\""+ random(50,100)
                    +"\"}",token);
                deserializeJson(doc, res_post);
                String data = doc["data"];
                String message = doc["message"];
                String status = doc["status"];
                if(status != "0"){
                    digitalWrite(motorDC,HIGH);
                }
                String Dataaa = "{\"User_id\":\"60f50845aefb7b6b73a1f636\",\"message\":\""+ message
                        +"\"}";
                Socket.emit("ResTimbangan",Dataaa.c_str());
                berat="0";
                cok = "";
                count = 100;
            }
        }
      }
  }
  
  if(millis() >= time_now + period){
      //async Handling reset count
      time_now += period;
      count = 0;
      digitalWrite(motorDC,LOW);
      if(count == 0){
          Serial.println("siap untuk scan");
      }
  }
  if(millis() >= time_now1 + second){
    //async Handling controll motor dc
      time_now1 += second;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration/58.2;
      if(distance <=20 && count == 100){
        analogWrite(motorDC,170);
      }
      if(distance <= 8 && count == 100){
        analogWrite(motorDC,0);
      }
  }
  Socket.loop();
}

// 77ffc933d1fbfdbf
// 77ffc933d1fb19bf
// 77ffc5bcbff5fb3
// 77ffc55be5ff6fef
// 77ffc55ebdd35f4b
// 77ffc55be5fb7bf1
// 77ffc55be5fb59f1
// 77ffc933d1fb11bd