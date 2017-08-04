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

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);

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
  Serial1.print(event.gyro.z);
  Serial1.print(";");
  
  delay(10);
}
