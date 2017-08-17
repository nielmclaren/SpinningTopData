//Serial test using the hardware uart on pins 0/1 (UART1).
//Connect an XBee and Teensy 3.1 to the adapter board
//
//Characters sent out the XBee terminal go:
// Onto the airwaves -> into UART1 RX -> out the serial monitor
//
//Characters sent out the serial monitor go:
// Out the UART1 TX pin -> onto the airwaves -> out the SBee serial terminal
//
//Be sure to select UART1 on the adapter board's switch for HW serial

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define ID 0

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);

Adafruit_NeoPixel strip;

long gyroTotal = 0;
long prevGyroTotal = 0;

int rgbw0 = strip.Color(64, 12, 42, 0);
int rgbw1 = strip.Color(18, 7, 39, 0);
int rgb0 = strip.Color(64, 12, 42);
int rgb1 = strip.Color(18, 7, 39);
int black = strip.Color(0, 0, 0);

void displaySensorDetails(void)
{
  sensor_t sensor;
  gyro.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    0x"); Serial.println(sensor.sensor_id, HEX);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" rad/s");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" rad/s");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" rad/s");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  if (ID == 2) {
    strip = Adafruit_NeoPixel(16, 23, NEO_GRBW + NEO_KHZ800);
  } else {
    strip = Adafruit_NeoPixel(27, 23, NEO_GRB + NEO_KHZ800);
  }
  
  Serial.begin(9600);
  Serial.println("Gyroscope Test");
  Serial.println("");

  /* Initialise the sensor */
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    /* There was a problem detecting the FXAS21002C ... check your connections */
    Serial.println("Ooops, no FXAS21002C detected ... Check your wiring!");
    while(1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  strip.begin();
  strip.show();
  
  // Begin HW serial to XBee
  Serial1.begin(9600);
}

void loop() {
  /* Get a new sensor event */
  sensors_event_t event;
  gyro.getEvent(&event);
/*
  // Display the results (speed is measured in rad/s)
  Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");
  Serial.println("rad/s ");
*/
  // Send results to XBee
  Serial1.print(ID);
  Serial1.print(":");
  Serial1.print(event.gyro.z);
  Serial1.print(";");
  
  if (ID == 2) {
    if (event.gyro.z < 1) {
      setStripColor(rgbw0);
    } else {
      setStripColor(rgbw1);
    }
  } else {
    gyroTotal += abs(event.gyro.z);
    
    int thresh = 100;
    if (gyroTotal > 2 * thresh) {
      if (prevGyroTotal <= 2 * thresh) {
        setStripColor(rgb1);
        gyroTotal = 0;
      }
    } else if (gyroTotal > thresh) {
      if (prevGyroTotal <= thresh) {
        setStripColor(rgb0);
      }
    }

    prevGyroTotal = gyroTotal;
  }

  strip.show();
  
  delay(50);
}

void setStripColor(uint32_t c) {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}
