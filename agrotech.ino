#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>
#include <WiFi.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

SoftwareSerial mySerial(26, 25);  // RX, TX

const char* ssid            = "muro";
const char* password        = "Piscok2000"; 

void setup() {
  Serial.begin(9600);
  mySerial.begin(4800);

  lcd.init();
  lcd.backlight();
  //Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi");
    lcd.setCursor(0, 1);
    lcd.print("Connecting to WiFi");

  }
  Serial.println("Connected to WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Connected to WiFi   ");
  delay(5000);
//Display
   lcd.setCursor(0, 0);
  lcd.print("PH   :              ");
  lcd.setCursor(0, 1);
  lcd.print("EC   :              ");
  lcd.setCursor(0, 2);
  lcd.print("Mois :              ");
  lcd.setCursor(0, 3);
  lcd.print("Temp :              ");
  lcd.setCursor(12,0 );
  lcd.print("N : ");
  lcd.setCursor(12,1 );
  lcd.print("P : ");
  lcd.setCursor(12,2 );
  lcd.print("K : ");
  delay(3000);

}

void loop() {

  byte queryData[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
  byte receivedData[19];

  mySerial.write(queryData, sizeof(queryData));  // Send the query data to the NPK sensor
  delay(1000);  // Wait for 1 second

  if (mySerial.available() >= sizeof(receivedData)) {  // Check if there are enough bytes available to read
    mySerial.readBytes(receivedData, sizeof(receivedData));  // Read the received data into the receivedData array

    // Parse and print the received data in decimal format
    unsigned int soilHumidity = (receivedData[3] << 8) | receivedData[4];
    unsigned int soilTemperature = (receivedData[5] << 8) | receivedData[6];
    unsigned int soilConductivity = (receivedData[7] << 8) | receivedData[8];
    unsigned int soilPH = (receivedData[9] << 8) | receivedData[10];
    unsigned int nitrogen = (receivedData[11] << 8) | receivedData[12];
    unsigned int phosphorus = (receivedData[13] << 8) | receivedData[14];
    unsigned int potassium = (receivedData[15] << 8) | receivedData[16];

    Serial.print("Soil Humidity: ");
    Serial.println((float)soilHumidity / 10.0);
    Serial.print("Soil Temperature: ");
    Serial.println((float)soilTemperature / 10.0);
    Serial.print("Soil Conductivity: ");
    Serial.println(soilConductivity);
    Serial.print("Soil pH: ");
    Serial.println((int)soilPH / 10.0);
    Serial.print("Nitrogen: ");
    Serial.println(nitrogen);
    Serial.print("Phosphorus: ");
    Serial.println(phosphorus);
    Serial.print("Potassium: ");
    Serial.println(potassium);

    lcd.setCursor(6,0);
   lcd.print("    ");
   lcd.setCursor(7,0);
   lcd.print((float)soilPH / 10.0);
   lcd.setCursor(6,1);
   lcd.print("    ");
   lcd.setCursor(7,1);
   lcd.print(soilConductivity);
   lcd.setCursor(6,2);
   lcd.print("    "); 
   lcd.setCursor(7,2);
   lcd.print((int)soilHumidity / 10);
   lcd.setCursor(10,2);
   lcd.print("%");
   lcd.setCursor(6,3);
   lcd.print("    ");
   lcd.setCursor(7,3);
   lcd.print((int)soilTemperature / 10);
   lcd.setCursor(10,3);
   lcd.print(char(223));
   lcd.print("C");
   lcd.setCursor(16,0);
   lcd.print("   ");
   lcd.setCursor(16,0);
   lcd.print(nitrogen);
   lcd.setCursor(16,1);
   lcd.print("   ");
   lcd.setCursor(16,1);
   lcd.print(phosphorus);
   lcd.setCursor(16,2);
   lcd.print("   ");
   lcd.setCursor(16,2);
   lcd.print(potassium);
   lcd.setCursor(15,3);
   lcd.print("mg/kg");
  }
}
