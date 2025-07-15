#include <Arduino_HS300x.h> // Temperatur + Luftfeuchtigkeit
#include <Arduino_LPS22HB.h> // Druck (theoretisch + Temperatur )
#include <ArduinoBLE.h>

// Definition von Service und Charakteristik
BLEService serialService("180C"); // Benutzerdefinierter Service
BLECharacteristic serialCharacteristic("2A56", BLERead | BLENotify, 512);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED einschalten, um zu zeigen, dass der Arduino läuft

  Serial.begin(9600);
  delay(2000); 

  // BLE-Initialisierung
  if (!BLE.begin()) {
    Serial.println("BLE konnte nicht gestartet werden");
    while (1); // Endlosschleife bei Fehler
  }
  BLE.setLocalName("Nano33BLE");         // Anzeigename im BLE-Scanner
  serialService.addCharacteristic(serialCharacteristic);
  BLE.addService(serialService);
  serialCharacteristic.writeValue("Initial Value");
  BLE.setAdvertisedService(serialService);
  if (!BLE.advertise()) {
    Serial.println("BLE-Werbung konnte nicht gestartet werden");
    while (1);
  }

  Serial.println("BLE ist bereit und bewirbt den Service 180C");

  // Initialisierung des Temperatursensors
  if (!HS300x.begin()) {
    Serial.println("Temperatursensor konnte nicht initialisiert werden");
    while (1); // Endlosschleife bei Fehler
  }

  // Intitialisierung des Drucksensors
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }

  digitalWrite(LED_BUILTIN, LOW);  // LED ausschalten, wenn der Arduino bereit ist
}

void loop() {
  // BLE-Zentrale (Central Device) verwalten
  BLEDevice central = BLE.central();
  if (central) {
    Serial.println("Verbindung hergestellt mit: " + central.address());
    int a = -20;
    while (central.connected()) {
      // Temperatur & Druck auslesen und senden
      float temperature = HS300x.readTemperature();
      float pressure = BARO.readPressure();
      String dataString = "Zeit:" + String(a) + " Temperatur:" + String(temperature) + "Druck:" + String(pressure);

      // Temperaturänderungen nur bei Bedarf senden
      serialCharacteristic.writeValue(dataString.c_str());
      Serial.println("Daten gesendet: " + dataString);

      delay(1000);
      a = a + 1;
    }
    Serial.println("Verbindung getrennt");
  }

  delay(500); // Vermeidung übermäßiger Belastung
}
