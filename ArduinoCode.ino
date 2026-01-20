#include <Volume.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Arduino.h>
#include <FastLED.h>

// Ik maak de variables voor de hoeveelheid leds en de datapins voor de leds
#define NUM_LEDS 44
#define DATA_PIN 3

CRGB leds[NUM_LEDS];

MPU6050 mpu;
Volume vol;

#define OUTPUT_READABLE_ACCELGYRO

// Ik maak de variables voor alle gyrosensor outputs

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  // Neopixel library boilerplate
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  vol.begin();

//
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#endif

  Serial.begin(9600); 

  Serial.println("Initializing MPU...");
  mpu.initialize();
  

  delay(100); 
  
  Serial.println("Testing MPU6050 connection...");
  if (mpu.testConnection() == false) {
    Serial.println("MPU6050 connection failed");

  } else {
    Serial.println("MPU6050 connection successful");
  }

// zet alle offsets voor de gyroscope sensor

  Serial.println("Updating internal sensor offsets...");
  mpu.setXAccelOffset(-1000); 
  mpu.setYAccelOffset(-1000); 
  mpu.setZAccelOffset(1500); 
  mpu.setXGyroOffset(50);   
  mpu.setYGyroOffset(50);   
  mpu.setZGyroOffset(50);   

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Krijg alle motion variables van de gyroscope sensor
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Map de frequentie met de ax variable van de gyroscope sensor voor de toonhoogte
  int tone_frequency = map(ax, -16384, 16384, 100, 1000); 

  // Map de volume met de ay variable van de gyroscope voor volume
  int raw_volume = constrain(ay, -8000, 8000); 
  int final_volume = map(raw_volume, -8000, 8000, 1, 255);
  
 // Zorgt dat de tone frequentie niet hoger dan 2000 kan en lager dan 50 kan
  tone_frequency = constrain(tone_frequency, 50, 2000); 
  
  // Zorgt dat het volume niet hoger dan 255 of lager dan 1 kan
  final_volume = constrain(final_volume, 1, 255); 

// Zet de tone en volume voor de speaker
  vol.tone(tone_frequency, final_volume);
//  maakt alle leds leeg voor de het volgend stukje code
  FastLED.clear(); 

// Map de leds met de kleur en number leds om te kijken hoeveel er aan moeten
  int leds_to_light = map(final_volume, 1, 255, 1, NUM_LEDS);

// Als het volume lager dan 5 is dan gaan 0 lichtjes aan
  if (final_volume < 5) {
    leds_to_light = 0;
  }
  
  // Kijken welke kleur aan moet gaan DIT IS OUD
  for (int l = 0; l < leds_to_light; l++) {
    if (l < NUM_LEDS / 3) {
      leds[l] = CRGB::Red; // Low volume
    } else if (l < (2 * NUM_LEDS) / 3) {
      leds[l] = CRGB::Red; // Medium volume
    } else {
      leds[l] = CRGB::Red; // High volume
    }
  }

// laat alle nodige leds branden en print debug logs naar console (sensor output voor testing)
  FastLED.show();

  Serial.print("Tone (Hz): ");
  Serial.print(tone_frequency);
  Serial.print(" | Volume (1-255): ");
  Serial.print(final_volume);
  Serial.print(" | LEDs Lit: ");
  Serial.println(leds_to_light);

//#endif
}