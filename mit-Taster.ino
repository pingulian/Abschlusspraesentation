#include <Arduino_HS300x.h> // Temperatur + Luftfeuchtigkeit
#include <Arduino_LPS22HB.h> // Druck (theoretisch + Temperatur )
#include <ArduinoBLE.h>

// Definition von Service und Charakteristik
BLEService serialService("180C"); // Benutzerdefinierter Service
BLECharacteristic serialCharacteristic("2A56", BLERead | BLENotify, 512);

String temperaturdata = "np.array([";
String druckdata = "np.array([";
int counter = 0;

void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

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

  //while (true) {

  //Taster 1
  //links
  //START / STOP
  bool zustandTaster = digitalRead(2); // Tasterzustand einlesen

  Serial.print("Zustand Taster: ");
  Serial.println(zustandTaster);

  if (zustandTaster == 0) {
    digitalWrite(LED_BUILTIN, HIGH);

    delay(3000);
      
    while (true) {
      float temperature = HS300x.readTemperature();
      float pressure = BARO.readPressure();
      //String dataString[i];
      //dataString[i] = " Temperatur:" + String(temperature) + "; Druck:" + String(pressure);

      temperaturdata += String(temperature);
      druckdata += String(pressure);

      Serial.println("for");

      delay(1000);

      zustandTaster = digitalRead(2); // Tasterzustand einlesen
      if (zustandTaster == 0) {
        temperaturdata +=" ])";
        druckdata +=" ])";
        digitalWrite(LED_BUILTIN, LOW);
        delay(3000);
        break;
      }

      temperaturdata +=", ";
      druckdata +=", ";

      counter = counter + 1;
    }


  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }


  //Taster 2
  //rechts
  //SENDEN

  bool zustandTaster2 = digitalRead(3); // Tasterzustand einlesen

  Serial.print("Zustand Taster: ");
  Serial.println(zustandTaster2);

  if (zustandTaster2 == 0) {

    //3x Blinken

    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    while(true) {
      // BLE-Zentrale (Central Device) verwalten
      BLEDevice central = BLE.central();
      if (central) {
        Serial.println("Verbindung hergestellt mit: " + central.address());
        //delay(10000);
        while (central.connected()) {
          String dataString = "Länge der Strings (0-basiert):" + String(counter);

          // Temperaturänderungen nur bei Bedarf senden
          serialCharacteristic.writeValue(dataString.c_str());
          Serial.println("Daten gesendet: " + dataString);

          String dataString2 = "Temperaturdaten:\n" + temperaturdata;

          // Temperaturänderungen nur bei Bedarf senden
          serialCharacteristic.writeValue(dataString2.c_str());
          Serial.println("Daten gesendet: " + dataString2);

          String dataString3 = "Druckdaten:\n" + druckdata;

          // Temperaturänderungen nur bei Bedarf senden
          serialCharacteristic.writeValue(dataString3.c_str());
          Serial.println("Daten gesendet: " + dataString3);

          delay(1000);
        }
      
      }
    }

  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
