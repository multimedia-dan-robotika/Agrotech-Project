// Source NPK tutorials 
// https://www.youtube.com/watch?v=nGvaAZSIQaA


#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial mySerial(26, 25);  // RX, TX

const char* ssid            = "*****";
const char* password        = "*****"; 

// settingan API ThingSpeak
unsigned long myChannelNumber = *******;
const char * myWriteAPIKey = "********";

WiFiClient client;

elapsedMillis IntervalSensor;

unsigned long setIntervalReadSensor = 2000;

// variable for result calibration
float dataTemp  ;
int dataConduct , datapH , dataNitrogen , dataPhosphorus , dataPotassium, dataSoil ;

void setup() {
  Serial.begin(9600);
  mySerial.begin(4800);
  ThingSpeak.begin(client);
  lcd.begin();
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
 if (IntervalSensor >= setIntervalReadSensor){
    sensorRead();    
    IntervalSensor = 0;  
  }
}

void sensorRead(){
  // Query data Address
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

// Perhitungan hasil kalibrasi sensor
    float dataSoil = soilHumidity / 10.0;
    float dataTemp = soilTemperature / 10.0;
    int dataConduct = soilConductivity;
    int datapH = soilPH / 10.0;
    int dataNitrogen = nitrogen;
    int dataPhosphorus = phosphorus / 2 ;
    int dataPotassium =potassium; 

//  Cetak Data ke Serial Monitor
    Serial.print("Soil Humidity: ");
    Serial.println(dataSoil);
    Serial.print("Soil Temperature: ");
    Serial.println(dataTemp);
    Serial.print("Soil Conductivity: ");
    Serial.println(dataConduct);
    Serial.print("Soil pH: ");
    Serial.println(datapH);
    Serial.print("Nitrogen: ");
    Serial.println(dataNitrogen);
    Serial.print("Phosphorus: ");
    Serial.println(dataPhosphorus);
    Serial.print("Potassium: ");
    Serial.println(dataPotassium);

    // Print LCD 
    lcd.setCursor(6,0);
    lcd.print("    ");
    lcd.setCursor(7,0);
    lcd.print(datapH);
    lcd.setCursor(6,1);
    lcd.print("    ");
    lcd.setCursor(7,1);
    lcd.print(dataConduct);
    lcd.setCursor(6,2);
    lcd.print("    "); 
    lcd.setCursor(7,2);
    lcd.print((int)dataSoil);
    lcd.setCursor(10,2);
    lcd.print("%");
    lcd.setCursor(6,3);
    lcd.print("    ");
    lcd.setCursor(7,3);
    lcd.print(dataTemp);
    lcd.setCursor(10,3);
    lcd.print(char(223));
    lcd.print("C");
    lcd.setCursor(16,0);
    lcd.print("   ");
    lcd.setCursor(16,0);
    lcd.print(dataNitrogen);
    lcd.setCursor(16,1);
    lcd.print("   ");
    lcd.setCursor(16,1);
    lcd.print(dataPhosphorus);
    lcd.setCursor(16,2);
    lcd.print("   ");
    lcd.setCursor(16,2);
    lcd.print(dataPotassium);
    lcd.setCursor(15,3);
    lcd.print("mg/kg");

    // Send data ke Thinkspeak
    SendThinkspeak(soilPH, dataConduct,dataSoil, dataTemp,dataNitrogen, dataPhosphorus, dataPotassium );
  }
}
void SendThinkspeak(float sensor1, float sensor2, int sensor3, int sensor4, int sensor5, int sensor6, int sensor7){
  
  float data1 = sensor1;
  float data2 = sensor2;
  int data3 = sensor3;
  int data4 = sensor4;
  int data5 = sensor5;
  int data6 = sensor6;
  int data7 = sensor7;

  ThingSpeak.setField(1, data1);
  ThingSpeak.setField(2, data2);
  ThingSpeak.setField(3, data3);
  ThingSpeak.setField(4, data4);
  ThingSpeak.setField(5, data5);
  ThingSpeak.setField(6, data6);
  ThingSpeak.setField(7, data7);

  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}
