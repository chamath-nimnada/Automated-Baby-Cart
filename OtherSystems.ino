#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>
#include <Servo.h>

// LCD Setup (I2C shared with RTC on A4/A5)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT11 Sensor Setup
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// DC Motor Setup
#define MOTOR_PIN 3

// RTC Setup
RTC_DS3231 rtc;

// Buzzer Setup
#define BUZZER_PIN 8

// Soil Moisture Sensor
#define SOIL_SENSOR A3

// Sound Sensor (Cry Detection)
#define SOUND_SENSOR 4

// Servo Motors for Cry Detection
Servo servo1, servo2;
#define SERVO1_PIN 5
#define SERVO2_PIN 6

// LDR Sensor (Gentle Light System)
#define LDR_SENSOR A0
#define LED_PIN 7

void setup() {
    Serial.begin(9600);
    Wire.begin();  // Use default I2C (A4/A5) for both LCD and RTC
    
    lcd.init();
    lcd.backlight();
    dht.begin();
    
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(SOIL_SENSOR, INPUT);
    pinMode(SOUND_SENSOR, INPUT);
    pinMode(LDR_SENSOR, INPUT);
    pinMode(LED_PIN, OUTPUT);

    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo1.write(0);
    servo2.write(0);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time
    }
}

void loop() {
    float temp = dht.readTemperature();
    int soil = analogRead(SOIL_SENSOR);
    int sound = digitalRead(SOUND_SENSOR);
    int light = analogRead(LDR_SENSOR);
    DateTime now = rtc.now();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");

    if (temp > 30) {
        digitalWrite(MOTOR_PIN, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("Fan: ON");
    } else {
        digitalWrite(MOTOR_PIN, LOW);
        lcd.setCursor(0, 1);
        lcd.print("Fan: OFF");
    }
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());

    if (now.hour() == 12 && now.minute() == 0) {
        digitalWrite(BUZZER_PIN, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("Feeding Time!");
        delay(3000);
        digitalWrite(BUZZER_PIN, LOW);
    }
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil: ");
    lcd.print(soil);

    if (soil < 500) {
        digitalWrite(BUZZER_PIN, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("Diaper Wet!");
        delay(3000);
        digitalWrite(BUZZER_PIN, LOW);
    }
    delay(2000);

    // Cry Detection System
    if (sound == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cry Detected!");
        servo1.write(90);
        servo2.write(90);
        delay(2000);
        servo1.write(0);
        servo2.write(0);
    }
    delay(2000);

    // Gentle Light System
    if (light < 300) {
        digitalWrite(LED_PIN, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light: ON");
    } else {
        digitalWrite(LED_PIN, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light: OFF");
    }
    delay(2000);
}
