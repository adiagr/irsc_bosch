#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

#include <FirebaseArduino.h>
#include <EEPROM.h>
#include <ESP8266WiFiMulti.h>

// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
const uint8_t fingerprint[20] = {0x53,0xDF,0xBD,0xD9,0x50,0x7E,0xD3,0x66,0x74,0x32,0x2C,0xBA,0x7D,0xC7,0xFE,0x28,0x7A,0xAC,0xD5,0x08};

ESP8266WiFiMulti WiFiMulti;
// Set these to run example.
#define FIREBASE_HOST "squadronirsc.firebaseio.com"
#define FIREBASE_AUTH "Gx832psg9aGgvoY6OrMmicHJFVpLMxzAaHHPQRR4"

#define VELOCITY 0
#define ACCELERATION 1
#define SEATBELT 2
#define ENGINE 3
#define LOCATION 4
#define ZIGZAG 5
#define RFID 6
#define TAMPER 7
#define OK 0
#define ERRORS 1


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("squadron", "@addy_19");
  while (WiFi.status() != WL_CONNECTED);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(1000);
  EEPROM.begin(512);
 /*First Run Code*/
}
void firstRunCheck(){


}

String MAC=WiFi.macAddress(); //This is a unique identifier, the MAC address. To prevent spoofing, this is to be used one time only.
 String parameter;
String getData(int choice)
{
  if(choice == VELOCITY || choice == ACCELERATION || choice == SEATBELT || choice == ENGINE || choice == LOCATION || choice == ZIGZAG) 
  {
    switch (choice) {
        case VELOCITY:
          Serial.println("VELOCITY");
          parameter="velocity";
          break;
        case ACCELERATION:
          Serial.println("ACCELERATION");
          parameter="acceleration";
          break;
        case SEATBELT:
          Serial.println("SEATBELT");
          parameter="seatbelt";
          break;

        case ENGINE:
          Serial.println("ENGINE");
          parameter="engine";
          break;

        case LOCATION:
          Serial.println("LOCATION");
          parameter="location";
          break;

        case ZIGZAG:
          Serial.println("ZIGZAG");
          parameter="zigzag";
          break;
        case RFID:
          Serial.println("RFID");
          parameter="rfid";
          break;
        case TAMPER:
        Serial.println("TAMPER");
        parameter="tamper";
        break;

        default:
         //
          break;
    }
    String error="error";
    float old_time=millis();
    while(!Serial.available()){
      if(millis()>old_time+10000)
      { /*Tampered, */
        return String("");
      }
    }
    return Serial.readString();
  }
  
  
}
String getUniqueID(){
  String x="JKU007";
  return x;
}
int sendDataToFirebase(String data)
{
 // Firebase.set("cooldown", 0);
 //   String pushInt(const String& path, int value);
 // String pushFloat(const String& path, float value);
 // String pushBool(const String& path, bool value);
  //virtual String pushString(const String& path, const String& value);
  //String push(const String& path, const JsonVariant& value);
    //void setInt/Float/Bool/String(const String& path, int value);
    //void getInt/Float/Bool/String(const String& path, int value);
  //  virtual void remove(const String& path);
  //Make paramter here
  
  String path="Vehicles/"+getUniqueID()+"/data/"+parameter;
  Firebase.setString(path, data);

  
}
bool checkAuthentication(String RFIDUI){
String path="/Vehicles/"+getUniqueID()+"/users";

FirebaseObject auth = Firebase.get(path);
JsonObject& obj = auth.getJsonVariant().as<JsonObject>();


if(obj.containsKey(RFIDUI))
  return true;
else
  return false;
//Apply AES here

}
bool checkFitnessWanted()
{
  if(Firebase.getInt("/Vehicles/"+getUniqueID()+"isRequested"))
    return true;
  else
    return false;
}

int getSpeed(String place)
{
  int allowedSpeed=Firebase.getInt("Places/"+place);
  //if(speed>allowedSpeed);
  int curSpeed=getData(VELOCITY).toInt();
  if(curSpeed>allowedSpeed)
  Firebase.pushString("Challans/"+getUniqueID()+"overspeed", String(curSpeed));
}

bool tamperDetect(){


  
}
bool pingCheck(){

}
String geolocation(String latlng){
    

    HTTPClient https;
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
client->setFingerprint(fingerprint);

    //Serial.print("[HTTP] begin...\n");
    if (https.begin(*client, "https://maps.googleapis.com/maps/api/geocode/json?key=AIzaSyDdEXxy2v7D1eiwiNRqRDIRCcf806WEd70&result_type=route&latlng="+latlng)) {  // HTTP


      //Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          //Serial.println(payload);
  DynamicJsonBuffer jsonBuffer(1024);

  // Parse JSON object
  JsonObject& doc = jsonBuffer.parseObject(payload);

  // Parse JSON object
 
    return doc["results"]["0"]["formatted_address"].as<String>();

        }
      } else {
       // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      https.end();
    }
}
/*
To do: 
1.A unique identifier for each device: MAC address in our case
2.Read data from nano: process it and send it, by default Firebase implements SSL
3.Framework to Assign a MAC address to a prticular user, how the fuck do I prevent spoofing though
4.For spoofing, I can send the MAC address on the first run to the server, return an encrypted string with concatenated RC and secret key. And use that string for all further purposes
Even a Hash would do --This is the difficult part though, will need to use the cloud functions for this
5.Check if the fitness request is set and update fitness variables
6.Get from nano and send acceleration, velocity, engine load and seatbelt status data to the cloud
7.Check if user is authenticated and send true or false to the nano
8.Send location of the vehicle to the cloud
9.Maps API to get road from location --Called geocoder, again difficult but can be implemented
10.Firebase to check speedlimit on the road
*/
void loop() {
  String temp=getData(RFID);
  if(temp.length()>0)
  {

  if(checkAuthentication(temp))
  { digitalWrite(12,HIGH);
  }
  
  }
  
  temp=getData(VELOCITY);
  sendDataToFirebase(temp);
    temp=getData(ACCELERATION);
  sendDataToFirebase(temp);
    temp=getData(SEATBELT);
  sendDataToFirebase(temp);
    temp=getData(ENGINE);
  sendDataToFirebase(temp);
    temp=getData(LOCATION);
  sendDataToFirebase(temp);
    temp=getData(ZIGZAG);
  sendDataToFirebase(temp);
    temp=getData(TAMPER);
  sendDataToFirebase(temp);
  }
  
