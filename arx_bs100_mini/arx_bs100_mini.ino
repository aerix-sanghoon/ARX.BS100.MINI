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

typedef struct _BLE_CONFIG{
  unsigned int scan_time; // seconds
  unsigned int scan_interval; // milli seconds;      
} BLE_CONFIG;

CTL_MESSAGE ctl_message;
BLEScan* pBLEScan;
BLE_CONFIG ble_config;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  ble_config.scan_time = 5;
  ble_config.scan_interval = 100;
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(ble_config.scan_interval);
  pBLEScan->setWindow(ble_config.scan_interval);  // less or equal setInterval value
   
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
  } else if(cmd=="scan"){
    response_doc["status"] = OK;
    
    BLEScanResults foundDevices = pBLEScan->start(ble_config.scan_time,false);
    int counts = foundDevices.getCount();
    for (int i=0;i<counts;i++){
      DynamicJsonDocument element_doc(1024);
      String element;  
      BLEAdvertisedDevice device = foundDevices.getDevice(i);
      BLEAddress ble_address = device.getAddress();
      String mac = ble_address.toString().c_str();
      String manufacturer_data = device.getManufacturerData().c_str();

      int rssi = device.getRSSI();
      response_doc[mac]["rssi"] = String(rssi);
      response_doc[mac]["manufacturerData"] = manufacturer_data;
    }
    serializeJson(response_doc,response);   
  }

  return response;
}

/**
BLEScanResults scan(){
  BLEScanResults foundDevices = pBLEScan->start(ble_config.scan_time,false);
  //pBLEScan->clearResults();

  return foundDevices;
}*/