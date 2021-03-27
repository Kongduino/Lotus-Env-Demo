#define Need_BMP
//#define Need_DHT
//#define Need_AHT

#include <ArduinoJson.h>
// http://librarymanager/All#ArduinoJSON

#ifdef Need_BMP
#include "Seeed_BMP280.h"
BMP280 bmp280;
#endif

#ifdef Need_DHT
#include <Wire.h>
#include "DHT.h"
// http://librarymanager/All#Grove_Temperature_And_Humidity_Sensor
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#endif

#ifdef Need_AHT
#include <Adafruit_AHT10.h>
// http://librarymanager/All#Adafruit_AHT10
Adafruit_AHT10 aht;
#endif

#define INTERVAL 5000
double t0;
uint32_t MSL = 101470; // Mean Sea Level in Pa

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\nWeather Station Test");

#ifdef Need_BMP
  if (!bmp280.init()) {
    Serial.println("Device not connected or broken!");
    while (1) delay(10);
    Serial.println("BMP inited!");
  }
#endif

#ifdef Need_DHT
  Wire.begin();
  dht.begin();
#endif

#ifdef Need_AHT
  if (! aht.begin()) {
    Serial.println("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 inited!");
#endif
}

void loop() {
  double t1 = millis();
  if (t1 - t0 > INTERVAL) {
#ifdef Need_BMP
    float p1;
    //get and print temperatures
    Serial.print("\nBMP280\nTemp: ");
    float t = bmp280.getTemperature();
    Serial.print(t);
    Serial.println(F("* C"));
    //get and print atmospheric pressure data
    Serial.print("MSL: ");
    Serial.print(MSL / 100.0);
    Serial.println(" HPa");
    Serial.print("Pressure: ");
    p1 = bmp280.getPressure();
    Serial.print(p1 / 100.0);
    Serial.println(" HPa");
    //get and print altitude data
    //get and print altitude data
    float a = bmp280.calcAltitude(MSL, p1, t);
    Serial.print("Altitude: ");
    Serial.print(a);
    Serial.println(" m");
#endif

#ifdef Need_DHT
    Serial.println("\nDHT11: ");
    float temp_hum_val[2] = {0};
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    if (!dht.readTempAndHumidity(temp_hum_val)) {
      Serial.print("Humidity: ");
      Serial.print(temp_hum_val[0]);
      Serial.print(" %\n");
      Serial.print("Temperature: ");
      Serial.print(temp_hum_val[1]);
      Serial.println(F("* C"));
    } else {
      Serial.println("Failed to get DHT temperature and humidity value.");
    }
#endif

#ifdef Need_AHT
    Serial.println("\nAHT10:");
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    Serial.print("Temperature: "); Serial.print(temp.temperature);
    Serial.println(F("* C"));
    Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("%");
#endif

    t0 = millis();
  }
  if (Serial.available()) {
    char mb[255];
    uint8_t ix = 0;
    while (Serial.available()) {
      mb[ix++] = Serial.read();
      delay(10);
    }
    mb[ix] = 0;
    Serial.println("Incoming:");
    Serial.println(mb);
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, mb);
    if (!error) {
      float newMSL = doc["MSL"];
      Serial.print("New MSL: "); Serial.println(newMSL);
      if (newMSL > 0.0) {
        MSL = newMSL * 100;
      }
    } else {
      Serial.println("Parse error!");
    }
  }
}
