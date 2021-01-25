#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad.h>
#define COLUMS 20
#define ROWS 4
#define PAGE ((COLUMS) * (ROWS))
#define fpSerial Serial3

const int ROW_NUM = 4;    //four rows
const int COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
uint8_t id;
int speed=0;
int lock = 1, auto_ = 0, fwd = 0, back = 0, lf = 0, rt = 0;
byte pin_rows[ROW_NUM] = {53, 51, 49, 47};      //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {45, 43, 41, 39}; //connect to the column pinouts of the keypad
char key;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fpSerial);
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
uint8_t getFingerprintEnroll()
{
  int p = -1;
  lcd.clear();
  lcd.print("place finter");
  Serial.println(id);
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
      Serial.println("Unknown error");
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
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Could not find fingerprint features");
    return p;
  default:
    Serial.println("Unknown error");
    return p;
  }

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
  lcd.clear();
  lcd.print("Place same finger");
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
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
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
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Could not find fingerprint features");
    return p;
  default:
    Serial.println("Unknown error");
    return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.print("Prints matched!");
  }
  /*else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		Serial.println("Communication error");
		return p;
	}
	else if (p == FINGERPRINT_ENROLLMISMATCH)
	{
		Serial.println("Fingerprints did not match");
		return p;
	}*/
  else
  {
    lcd.clear();
    lcd.print("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.print("Stored!");
    delay(1000);
  }
  /*else if (p == FINGERPRINT_PACKETRECIEVEERR)
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
	}*/
  else
  {
    lcd.clear();
    lcd.print("saving error");
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
    lcd.print("Enter ID # 1-127");
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
          lcd.print("Enter ID # 1-127");
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
    
    if (speed > 255)
    {
      lcd.clear();
      lcd.print("Max 255 - canceled");
    }
    else
    {
      speed = input_speed.toInt();
      lcd.clear();
      lcd.print("Going speed ");
      lcd.print(speed);
    }
  }
}
void setup()
{
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
  //read lock
  //set speed to 0
  if (!lock)
  {
    int known = getFingerprintIDez();
    if (known != -1)
    {
      while (!lock)
      {
        if (!auto_)
        {
          //set speed to 0
        }
        key = keypad.getKey();
        if (key == 'A')
        {
          lcd.clear();
          lcd.print(key);
          enrole();
          lcd.clear();
          lcd.print("End Enrole");
        }

        if (key == 'D')
        {
          //take input fn
          auto_ = 1;
        }
        //read buttons
        while (fwd)
        { //read button
        }
        while (back)
        { //read button
        }
        while (lf)
        { //read button
        }
        while (rt)
        { //read button
        }
        //read lock
      }
    }
    lcd.setCursor(0, 1);
    lcd.print(" Welcome ");
  }
}