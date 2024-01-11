#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <Servo.h>
Servo myservo;
int led = 5;
int pos;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // Change to (0x27,20,4) for 20x4 LCD.
int angle[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

#define TRIGGER_PIN  A1  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A0  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PINB  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PINB     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
//#define TRIGGER_PINC  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.
//#define ECHO_PINC     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonarA(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarB(TRIGGER_PINB, ECHO_PINB, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//NewPing sonarC(TRIGGER_PINC, ECHO_PINC, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  lcd.init();
  lcd.backlight();
  myservo.attach(2);
  pinMode(led, OUTPUT);
  Serial.print("initializing ");
  myservo.write(0);
  lcd.setCursor(0, 0);

  lcd.print(".....SYSTEM......");
  lcd.setCursor(0, 1);
  lcd.print("....BOOTING......");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(".....SMART......");
  lcd.setCursor(0, 1);
  lcd.print("...WASTE-BIN....");
  delay(3000);
  lcd.clear();
  myservo.write(10);
}

void loop() {
  delay(100);                      // Wait 100ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int distanceA = sonarA.ping(); // Send ping, get ping time in microseconds (uS).
  int distanceAA = distanceA / US_ROUNDTRIP_CM;
  unsigned int distanceB = sonarB.ping(); // Send ping, get ping time in microseconds (uS).
  int distanceBB = distanceB / US_ROUNDTRIP_CM;
  //  unsigned int distanceC = sonarC.ping(); // Send ping, get ping time in microseconds (uS).
  //  int distanceCC = distanceC / US_ROUNDTRIP_CM;

  Serial.print("PingA: ");
  Serial.print(distanceAA); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
  Serial.print("PingBB: ");
  Serial.print(distanceBB); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  //  Serial.println("cm");
  //  Serial.print("PingCC: ");
  //  Serial.print(distanceCC); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  //  Serial.println("cm");
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMART");
  lcd.print("WASTE-BIN");
  lcd.setCursor(0, 1);
  lcd.print("MOVE CLOSER");

  if (distanceBB > 7) {
    Serial.print("first "); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    digitalWrite(led, LOW);
    if (distanceAA < 15) {
      Serial.print("waste-bin opened ");
      lcd.clear();
      digitalWrite(led, LOW);
      lcd.setCursor(0, 0);
      lcd.print("WASTE-BIN OPENED");
      Serial.println("servo on");
      for (int i = 0; i < 10; i++)
      {
        Serial.print("waste-bin closed ");
        myservo.write(angle[i]);
        delay(20);
      }
      delay(5000);
      for (int i = 10; i > 0; i--)
      {
        myservo.write(angle[i]);
        delay(20);
      }
      delay(500);
      lcd.clear();
    }
    else {
      lcd.clear();
      myservo.write(0);

      Serial.println("servo off");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MOVE CLOSER TO ");
      //lcd.print(distanceAA);
      //lcd.print("cm");
      lcd.setCursor(0, 1);
      lcd.print("DUMP DIRT");
      //      lcd.print(distanceBB);
      //      lcd.print("cm");
      //      lcd.setCursor(12, 1);
      //      lcd.print("cm");
      delay(1000);
      //lcd.clear();
    }
  }
  else {
    lcd.clear();
    Wastefull();
    myservo.write(0);
    Serial.println("bin is full");
    lcd.setCursor(0, 0);
    lcd.print("waste-bin full");
    lcd.setCursor(0, 1);
    lcd.print("SMS SENT");
    digitalWrite(led, HIGH);
    Send_Sms();
    //delay(1000);

  }
  delay(1000);
}


void Send_Sms() {
  //Being serial communication witj Arduino and SIM800
  Serial.begin(9600);
  delay(1000);

  //Set SMS format to ASCII
  Serial.write("AT+CMGF=1\r\n");
  delay(1000);

  //Send new SMS command and message number
  Serial.write("AT+CMGS=\"\"\r\n");
  delay(1000);

  //Send SMS content
  Serial.write("The bin is full, need to be dispose");
  delay(1000);

  //Send Ctrl+Z / ESC to denote SMS message is complete
  Serial.write((char)26);
  delay(1000);

}

void Wastefull() {
  //Being serial communication witj Arduino and SIM800
  Serial.begin(9600);
  delay(1000);

  //Set SMS format to ASCII
  Serial.write("AT+CMGF=1\r\n");
  delay(1000);

  //Send new SMS command and message number
  Serial.write("AT+CMGS=\"phone number\"\r\n");
  delay(1000);

  //Send SMS content
  Serial.write("The bin is full, need to be dispose");
  delay(1000);

  //Send Ctrl+Z / ESC to denote SMS message is complete
  Serial.write((char)26);
  delay(1000);

}
