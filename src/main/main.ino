#include <Arduino.h>
#include "Adafruit_Sensor.h"
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include "Adafruit_Fingerprint.h"
#include "Keypad.h"
#define COLUMS 20
#define ROWS 4
#define PAGE ((COLUMS) * (ROWS))
#define fpSerial Serial1
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
long duration;    // variable for the duration of sound wave travel
int distance;     // variable for the distance measurement

const int ROW_NUM = 4;    //four rows
const int COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
uint8_t id;
int speed = 0;
int lock = 0, auto_ = 0, fwd = 0, back = 0, lf = 0, rt = 0;
byte pin_rows[ROW_NUM] = {53, 51, 49, 47};      //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {45, 43, 41, 39}; //connect to the column pinouts of the keypad
char key;
LiquidCrystal_I2C lcd(0x27, COLUMS, ROWS);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fpSerial);
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
uint8_t getFingerprintEnroll()
{

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  lcd.clear();
  lcd.print("Place Finger");
  delay(100);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
    {
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("error0");
      delay(100);
    }
    break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Image converted");
    break;
  // case FINGERPRINT_IMAGEMESS:
  //   Serial.println("Image too messy");
  //   return p;
  // case FINGERPRINT_PACKETRECIEVEERR:
  //   Serial.println("Communication error");
  //   return p;
  // case FINGERPRINT_FEATUREFAIL:
  //   Serial.println("Could not find fingerprint features");
  //   return p;
  // case FINGERPRINT_INVALIDIMAGE:
  //   Serial.println("Could not find fingerprint features");
  //   return p;
  default:
  {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("error");
    delay(100);
  }
    return p;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("Place same finger again");
  delay(100);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    // case FINGERPRINT_PACKETRECIEVEERR:
    //   Serial.println("Communication error");
    //   break;
    // case FINGERPRINT_IMAGEFAIL:
    //   Serial.println("Imaging error");
    //   break;
    default:
    {
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("error2");
      delay(100);
    }
    break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Image converted");
    break;
  // case FINGERPRINT_IMAGEMESS:
  //   Serial.println("Image too messy");
  //   return p;
  // case FINGERPRINT_PACKETRECIEVEERR:
  //   Serial.println("Communication error");
  //   return p;
  // case FINGERPRINT_FEATUREFAIL:
  //   Serial.println("Could not find fingerprint features");
  //   return p;
  // case FINGERPRINT_INVALIDIMAGE:
  //   Serial.println("Could not find fingerprint features");
  //   return p;
  default:
  {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("error3");
    delay(100);
  }
    return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Prints matched!");
    lcd.clear();
    lcd.print("matched");
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    lcd.clear();
    lcd.print("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_ENROLLMISMATCH)
  {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.print("Fingerprints did not match");
    delay(3000);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Unknown error");
    delay(100);
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Stored!");
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Could not store in that location");
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Error writing to flash");
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
    return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
void enrole()
{
  String input_id;
  while (1)
  {
    lcd.clear();
    lcd.print("Enter ID # 1-900");
    lcd.setCursor(0, 1);
    lcd.print(input_id);
    while (1)
    {
      key = keypad.getKey();
      if (key)
      {
        if (key == '#')
        {

          Serial.println("Break");
          break;
        }
        else if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
        {
          input_id += key;
          lcd.clear();
          lcd.print("Enter ID # 1-900");
          lcd.setCursor(0, 1);
          lcd.print(input_id);
        }
        else if (key == 'C')
        {

          break;
        }
      }
    }
    if (key == 'C')
    {

      lcd.clear();
      lcd.print("Cancel");
      break;
    }
    id = input_id.toInt();
    if (id == 0)
    {
      lcd.clear();
      lcd.print("0 not Allowed");
    }
    else
    {
      lcd.clear();
      lcd.print("Enrolling ID # ");
      lcd.print(id);
      while (!getFingerprintEnroll())
        ;
      input_id = "";
    }
  }
}
void set_speed()
{
  String input_speed;
  while (1)
  {
    lcd.clear();
    lcd.print("Enter speed # 0-255");
    while (1)
    {
      key = keypad.getKey();
      if (key)
      {
        if (key == '#')
        {

          Serial.println("Break");
          break;
        }
        else if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
        {
          input_speed += key;
          lcd.clear();
          lcd.print("Enter speed # 0-255");
          lcd.setCursor(0, 1);
          lcd.print(input_speed);
        }
        else if (key == 'C')
        {
          lcd.clear();
          lcd.print("Cancel");
          break;
        }
      }
    }
    if (key == 'C')
    {
      lcd.clear();
      lcd.print("Cancel");
      break;
    }
    speed = input_speed.toInt();
    if (speed > 255)
    {
      lcd.clear();
      lcd.print("Max 255 - canceled");
      break;
    }
    else
    {

      lcd.clear();
      lcd.print("Going speed ");
      go(1);
      lcd.print(speed);
      break;
    }
  }
}
void go(int x)
{
  if (x == 0)
  {
    //stop
  }
  if (x == 1)
  {
    //forward
  }
  if (x == 2)
  {
    //back
  }
  if (x == 3)
  {
    //left
  }
  if (x == 4)
  {
    //right
  }
}
void read_lock()
{
  //lock=digitalRead(...);
}
int read_sonar()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  if (distance < 6)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void setup()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);  // Sets the echoPin as an INPUT
  pinMode(37, OUTPUT);
  pinMode(50, OUTPUT);
  Serial.begin(115200);
  Serial.println("Home Automation");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
  }
  else
  {
    Serial.println("Did not find fingerprint sensor :(");
    //while (1)
    ;
  }

  lcd.init();
  lcd.backlight();
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Hello ");
}

void loop()
{
  speed = 0;
  go(0);
  delay(100);
  while (lock)
  {
    read_lock();
    //touch_button()
  }
  read_lock();

  if (!lock)
  {
    int known = getFingerprintIDez();
    if (known != -1)
    {
      lcd.clear();
      while (!lock)
      {
        delay(100);

        if (known == 1)
        {

          key = keypad.getKey();
          if (key == 'A')
          {
            lcd.clear();
            lcd.print(key);
            enrole();
            lcd.clear();
            lcd.print("End Enrole");
            delay(2000);
          }
          if (key == 'C')
          {
            break;
          }
          lcd.clear();
          lcd.print("A for Enrole");
        }

        else
        {
          lcd.clear();
          lcd.print("Stop");
          lcd.setCursor(0, 1);
          lcd.print("speed: ");
          lcd.print(speed);
          while (!lock)
          {
            key = keypad.getKey();
            if (key == '0')
            {
              speed = 0;
              go(0);
              lcd.clear();
              lcd.print("Stop");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
            }
            if (key == '*')
            {
              set_speed();

              lcd.clear();
              lcd.print("Forward");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
              auto_ = 1;
            }
            //read buttons
            if (key == 'A')
            {
              //read button
              auto_ = 0;
              speed = 100;
              go(1);
              lcd.clear();
              lcd.print("Forward");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
              delay(100);
            }
            if (key == 'B')
            { //read button
              auto_ = 0;
              speed = 100;
              go(2);
              lcd.clear();
              lcd.print("Back");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
            }
            if (key == 'C')
            { //read button
              auto_ = 0;
              speed = 100;
              go(3);
              lcd.clear();
              lcd.print("Left");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
            }
            if (key == 'D')
            { //read button
              auto_ = 0;
              speed = 100;
              go(4);
              lcd.clear();
              lcd.print("Right");
              lcd.setCursor(0, 1);
              lcd.print("speed: ");
              lcd.print(speed);
            }
            int too_close = read_sonar();
            if (too_close)
            {
              go(0);
            }
            else if (auto_)
            {
              go(1);
            }
            read_lock();
          }
        }
        read_lock();
      }
    }
    else
    {
      lcd.clear();
      lcd.print(" No Match ");
      delay(2000);
    }
  }
  lcd.clear();
  lcd.print(" Welcome ");
}
