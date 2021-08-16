#include "httpclient.h"
#include <ESP8266HTTPClient.h>
String httpPostToENDpoint(String endPoint,String body,String token){
    WiFiClient client;
    HTTPClient http;
    http.begin(client,"http://192.168.10.111:3004/kambing/"+endPoint);
    http.addHeader("Content-Type", "application/json");  
    if(validatingEndpoint(endPoint) == true){
        Serial.println(token);
        http.addHeader("Authorization",token);
    }
    int httpCode = http.POST(body);
    String payload = http.getString();
    Serial.println(httpCode);
    return payload;
}

int validatingEndpoint(String enpoint){
    String AuthEndpoint [] = {"id-tag","weight-got"};
    for(int i=0;i<sizeof AuthEndpoint/sizeof AuthEndpoint[0];i++){
        if(AuthEndpoint[i] == enpoint){
            return true;
        }
    }
    return false;
}