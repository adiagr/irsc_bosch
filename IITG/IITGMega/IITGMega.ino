
#include <TinyGPS.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN 53 // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);
TinyGPS gps;

void setup(){
	Serial.begin(9600); //Node
	Serial1.begin(9600); //GPS
	Serial2.begin(9600); //Nano
	SPI.begin();  //RFID
	mfrc522.PCD_Init();		// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}
float velocity, GPSvelocity, xAcc, yAcc, zAcc, lati, longi;
int seatbeltStatus, zigzagStatus;
String input;
unsigned long age;
String output;


static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}



void loop()
{
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	else if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
	else
	{
		// Dump debug info about the card; PICC_HaltA() is automatically called
		mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

	}	
	
	if(Serial.available())
	{	input = Serial.readString();
		if(input.equals("VELOCITY"))
		{
			Serial2.println("ACCELERATION");
			while(!Serial2.available());
			output=Serial2.readString();
		}

		else if(input.equals("LOCATION"))
		{
			gps.satellites();
			gps.hdop();
			gps.f_get_position(&lati, &longi, &age);
			GPSvelocity=gps.f_speed_kmph();
			output=String(lati)+","+String(longi);
			
		}
		else if(input.equals("RFID"))
		{
			
		}
		else if(input.equals("TAMPER"))
		{
			output=String("Tamper");
		}
		else if(input.equals("ACCELERATION"))
		{
			Serial2.println("ACCELERATION");
			while(!Serial2.available());
			output=Serial2.readString();
		}
		else if(input.equals("SEATBELT"))
		{
			Serial2.println("SEATBELT");
			while(!Serial2.available());
			output=Serial2.readString();
		}
		else if(input.equals("ZIGZAG"))
		{
			Serial2.println("ZIGZAG");
			while(!Serial2.available());
			output=Serial2.readString();
		}

	}


}
