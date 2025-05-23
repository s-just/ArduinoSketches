#include "DHT.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include <WiFiS3.h>
#include "arduino_secrets.h" // This file holds SSID/Password information, not included in the repo, declare the headers yourself to run.

#define DHTTYPE DHT11
#define DHTPIN 2

DHT dht(DHTPIN, DHTTYPE);
ArduinoLEDMatrix matrix;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

const char* serverAddress = "10.0.0.3";
const int serverPort = 8000;

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}

  dht.begin();
  matrix.begin();

  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // Check firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Upgrade firmware");
  }

  // Attempt to connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds
    delay(10000);
  }

  // Free from loop and connected
  Serial.print("Connected!");
  printCurrentNet();
  printWifiData();
}

void loop() {
  delay(2000); // Reading delay

  // Read humidity and temperature from the sensor/module
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT sensor");
    return;
  }

  Serial.print("Humidity: "); Serial.print(h); Serial.println("%");
  Serial.print("Temperature: "); Serial.print(t); Serial.println("C");

  // Send data over Wi-Fi
  sendData(t, h);

  // Display on LED matrix
  matrix.clear();
  String tempString = String(t, 0);
  String humidityString = String(h, 0);
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println("    TEMP: " + tempString + "C" + "    HUMIDITY: " + humidityString + "%");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void sendData(float temperature, float humidity) {
  WiFiClient client;

  // Attempt to connect to the server
  if (!client.connect(serverAddress, serverPort)) {
    Serial.print("Connection to server failed! IP: ");
    Serial.print(serverAddress);
    Serial.print(" Port: ");
    Serial.println(serverPort);
    return;
  }

  Serial.println("Connected to server!");

  // Create the HTTP POST request body
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);

  // Send HTTP POST request
  client.print("POST /data HTTP/1.1\r\n"); // Python server listens at /data
  client.print("Host: ");
  client.print(serverAddress);
  client.print("\r\n");
  client.print("Content-Type: application/x-www-form-urlencoded\r\n");
  client.print("Content-Length: ");
  client.print(postData.length());
  client.print("\r\n\r\n"); // End of headers
  client.print(postData);
  client.print("\r\n");

  // Read response from server to serial
  unsigned long timeout = millis();
  while (client.connected() && (millis() - timeout < 5000)) {
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      timeout = millis();
    }
  }

  Serial.println("\nClosing connection.");
  client.stop(); // Close the connection
}

void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
  
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}
