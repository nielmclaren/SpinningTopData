
import processing.serial.*;

Serial serial;
String buffer;
float accelX;
float accelY;
float accelZ;

ArrayList<Reading> accelZReadings;
int maxReadings;
float maxRadiansPerSecond;

void setup() {
  size(800, 800);
 
  printArray(Serial.list());
  serial = new Serial(this, Serial.list()[1], 9600);
  buffer = "";

  accelX = 0;
  accelY = 0;
  accelZ = 0;

  accelZReadings = new ArrayList<Reading>();
  maxReadings = 100;
  maxRadiansPerSecond = 40;
}

void draw() {
  background(0);

  readData(serial);
  drawValues();
  drawReadings();
}

void readData(Serial serial) {
  while (serial.available() > 0) {
    char c = (char)serial.read();
    if (c == ';') {
      readAccel(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}

void readAccel(String values) {
  String[] parts = values.split(",");
  if (parts.length < 3) {
    println("Got bad value: " + values);
    return;
  }

  boolean isError;
  try {
    accelX = Float.parseFloat(parts[0]);
    accelY = Float.parseFloat(parts[1]);
    accelZ = Float.parseFloat(parts[2]);
    isError = false;
  } 
  catch (Exception e) {
    println("Failed to parse: " + values);
    isError = true;
  }

  accelZReadings.add(new Reading(accelZ, isError));
  if (accelZReadings.size() > maxReadings) {
    accelZReadings.remove(0);
  }
}

void drawValues() {
  rectMode(CORNER);
  noFill();
  stroke(255);
  rect(0, 0, width, height/2);

  fill(255);
  stroke(0);
  strokeWeight(1);

  rectMode(CORNERS);
  rect(0 * width/3, height/4, 1 * width/3, height/4 + map(accelX, -maxRadiansPerSecond, maxRadiansPerSecond, height/4, -height/4));
  rect(1 * width/3, height/4, 2 * width/3, height/4 + map(accelY, -maxRadiansPerSecond, maxRadiansPerSecond, height/4, -height/4));
  rect(2 * width/3, height/4, 3 * width/3, height/4 + map(accelZ, -maxRadiansPerSecond, maxRadiansPerSecond, height/4, -height/4));

  String accelXText = accelX + " m/s^2";
  String accelYText = accelY + " m/s^2";
  String accelZText = accelZ + " m/s^2";

  fill(128);
  text(accelXText, 1 * width/6 - textWidth(accelXText), height/4);
  text(accelYText, 3 * width/6 - textWidth(accelYText), height/4);
  text(accelZText, 5 * width/6 - textWidth(accelZText), height/4);
}

void drawReadings() {
  rectMode(CORNER);
  noFill();
  stroke(255);
  rect(0, height/2, width, height/2);

  stroke(0);
  strokeWeight(1);

  rectMode(CORNERS);

  if (accelZReadings.size() > 0) {
    float w = width / accelZReadings.size();

    for (int i = 0; i < accelZReadings.size(); i++) {
      Reading reading = accelZReadings.get(i);
      float h = map(reading.value, -maxRadiansPerSecond, maxRadiansPerSecond, -height/4, height/4);
      
      if (reading.isError) {
        fill(255, 0, 0, 16);
      } else {
        fill(255, 16);
      }
      
      rect(w * i, height/2, w * (i + 1), height);
      
      if (reading.isError) {
        fill(255, 0, 0);
      } else {
        fill(255);
      }
      
      rect(w * i, height*3/4, w * (i + 1), height*3/4 + h);
    }
  }
}