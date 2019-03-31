#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

long timer = 0;

String input; //Initialized variable to store recieved data
int pot_speed, pot_engine, pot_acceleration;
float xacc, yacc, zacc;
void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

}

void loop() {
  //Read the serial data and store in var
  // Serial.println(mystr); //Print data on Serial Monitor
  delay(1000);
  int output;
  if (Serial.available())
  { input = Serial.readString();
    if (input.equals("VELOCITY"))
    {
      pot_speed = analogRead(A0);

      output = pot_speed;
    }
    else if (input.equals("ENGINE LOAD"))
    {
      pot_engine = analogRead(A1);

      output = pot_engine;
    }

    else if (input.equals("ACCELERATION"))
    {
      pot_acceleration = analogRead(A2);
      output = pot_acceleration;
    }
    else if (input.equals("SEATBELT"))
    {
      output = digitalRead(3);
    }
    else if (input.equals("COOLANT"))
    {
      output = digitalRead(4);
    }
    else if (input.equals("ZIGZAG"))
    {
      int test = zigzak_detect();
      if (test == 2)
      {
        output = HIGH;
      }
      else
      {
        output = LOW;
      }
      //output = Serial2.readString();
    }

  }
}

int zigzak_detect()
{
  mpu6050.update();
  xacc = mpu6050.getAccX();
  yacc = mpu6050.getAccY();
  zacc = mpu6050.getAccZ();

int  res = sqrt(pow(xacc, 2) + pow(yacc, 2));

        if (res > 1.2 || res < -1.2)
  {
    return 2; //zig zak driving detected
  }
  else
  {
    return 1;
  }

}
