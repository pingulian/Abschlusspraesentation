#include <Arduino_BMI270_BMM150.h>

String dataStringt = "t (in s): ";
String dataStringx = "x (in g): ";
String dataStringy = "y (in g): ";
String dataStringz = "z (in g): ";
unsigned long lastMeasurement = 0;
const unsigned long interval = 100;         // Intervall in Millisekunden
float timer = 0;

void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("IMU konnte nicht gestartet werden!");
    while (1);
  }

  Serial.println("IMU erfolgreich gestartet!");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastMeasurement >= interval) {
    lastMeasurement = currentMillis;

    float x, y, z;

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);

      timer += interval;
      dataStringt += String(timer / 1000) + ", ";
      dataStringx += String(x, 3) + ", ";
      dataStringy += String(y, 3) + ", ";
      dataStringz += String(z, 3) + ", ";
      // Ausgabe zur Kontrolle
      Serial.print("Messung: ");
      Serial.print("ax=");
      Serial.print(x, 2);
      Serial.print("g, ay=");
      Serial.print(y, 2);
      Serial.print("g, az=");
      Serial.print(z, 2);
      Serial.println("g");

      Serial.println(dataStringt);
      Serial.println(dataStringx);
      Serial.println(dataStringy);
      Serial.println(dataStringz);


      
    }
  }

  bool zustandTaster = digitalRead(2); // Tasterzustand einlesen
  if (zustandTaster == 0) {
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println(dataStringt);
      Serial.println(dataStringx);
      Serial.println(dataStringy);
      Serial.println(dataStringz);
      delay(5000);
    }
  }
}
