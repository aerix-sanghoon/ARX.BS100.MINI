#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <stdio.h>
#include <ArduinoJson.h>

#define OK                            0
#define ERR_INVALID_JSON_FORMAT       100
#define ERR_CMD_NOT_FOUND             200

/**
  Additional Library 
  
  1. ArduinoJson by Benoit Blanchon Version 6.19.4
    : reference site
      https://arduinojson.org/v6/doc/upgrade/
*/

typedef struct _CTL_MESSAGE {
  String request;
  String response;
} CTL_MESSAGE;

typedef struct _DEVICE {
  byte mac[6];
} DEVICE;

typedef struct _DEVICE_LIST {
  DEVICE device[10];    
} DEVICE_LIST;

CTL_MESSAGE ctl_message;
BLEScan* pBLEScan;
const int json_capacity = JSON_OBJECT_SIZE(5);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    String message = Serial.readStringUntil('\n');
    String response = parser(message);
    Serial.println(response);
  }
  
}

String parser(String input){
  String response;

  DynamicJsonDocument request_doc(1024);
  DynamicJsonDocument response_doc(2048);  
  
  auto error = deserializeJson(request_doc,input);
  if(error){
    response_doc["status"] = ERR_INVALID_JSON_FORMAT;
    serializeJson(response_doc,response);    
    return response;
  } 

  String cmd = request_doc["cmd"];
  if(cmd=="null"){
    response_doc["status"] = ERR_CMD_NOT_FOUND;
    serializeJson(response_doc,response);    
    return response;
  }
}