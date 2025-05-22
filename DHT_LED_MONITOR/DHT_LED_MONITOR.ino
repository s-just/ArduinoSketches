#include "DHT.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#define DHTTYPE DHT11
#define DHTPIN 2

DHT dht(DHTPIN, DHTTYPE);
ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(1000);

  matrix.begin();
}

void loop() {
  delay(2000);
  // Read humidity and temperature from the sensor/module
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT");
    return;
  }

  Serial.print("Humidity: "); Serial.print(h); Serial.println("%"); 
  Serial.print("Temperature: "); Serial.print(t); Serial.println("°C");

  matrix.clear();

  // Convert temperature/humidity float to string format
  String tempString = String(t, 0);
  String humidityString = String(h, 0);

  //  Display the info on LED Matrix
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);

  matrix.textScrollSpeed(100);

  // Font_5x7 is standard for led digits
  matrix.textFont(Font_5x7);

  // Begin text:
  // (x, y, color)
  // The UNO R4 WiFi matrix is 12x8 pixels
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println("    TEMP: " + tempString + "C" + "    HUMIDITY: " + humidityString + "%");
  matrix.endText(SCROLL_LEFT); // Solves the issue of not being able to fit the text on the LED matrix normally

  matrix.endDraw();
}