#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define RE 17
#define DE 16
SoftwareSerial mod(26,25);

const char* ssid            = "muro";
const char* password        = "Piscok2000";


// ===================================================================
//                          TUTORIAL
// const byte temp[] = {0x01,0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xcf};//
// const byte mois[]  = {0x01,0x03,0x00,0x12,0x00,0x01,0x24,0x0F};
// const byte econ[] = {0x01,0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xce};
// const byte ph[] = {0x01,0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b};//0x0B64
// const byte nitro[] = { 0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C };
// const byte phos[] = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
// const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };
// =====================================================================

const byte mois[] = {0x01, 0x03, 0x08, 0x02, 0x92, 0x57, 0xB6};
const byte temp[] = {0x01, 0x03, 0x08, 0xFF, 0x9B, 0x57, 0xB6};
const byte econ[] = {0x01, 0x03, 0x08, 0x03, 0xE8, 0x57, 0xB6};
const byte ph[] = {0x01, 0x03, 0x08, 0x00, 0x38, 0x57, 0xB6};
const byte nitro[]={0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[]={0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
// const byte kal[]    = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
byte values[11];
byte MoisValue, TempValue, ECValue, PHValue, NValue, PValue, KValue;
byte SensorReading = 1;


//                    TUTORIAL
float envhumidity = 0.0, envtemperature = 0.0, soil_ph = 0.0, soil_mois = 0.0, soil_temp = 0.0;
byte val1 = 0, val2 = 0, val3 = 0, val4 = 0,val5 = 0, val6 = 0, val7 = 0;

const unsigned long intervalInput   = 3000;   // Interval baca sensor (ms)
const unsigned long intervalOutput  = 1000;   // Interval print LCD (ms)
const unsigned long intervalUpload  = 20000;  // Interval kirim ke Firebase (ms)

elapsedMillis elapsedInput;
elapsedMillis elapsedOutput;
elapsedMillis elapsedUpload;

void setup() {
  lcd.begin();
  lcd.backlight();
  //Connect Wifi
  Serial.begin(115200);
  mod.begin(4800);
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
//LCD SET
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

pinMode(RE, OUTPUT);
pinMode(DE, OUTPUT);

}

//        TUTORIAL 
void loop() {
  // put your main code here, to run repeatedly:
  val1 = moisture();
  soil_mois = val1/1.8;
  delay(1000);
  soil_temp = temperature()/10.0;
  delay(1000);
  val3 = econduc();
  delay(1000);
  val4 = phydrogen()/25;
  soil_ph = val4;
  delay(1000);
  val5 = nitrogen();
  delay(1000);
  val6 = phosphorous();
  delay(1000);
  val7 = potassium();
  delay(1000);

  Serial.print("Moisture: ");
  Serial.print(soil_mois);
  Serial.println(" %");
  delay(1000);
  Serial.print("Temperature: ");
  Serial.print(soil_temp);
  Serial.println(" C");
  delay(1000);
  Serial.print("EC: ");Serial.print(val3);Serial.println(" us/cm");
  delay(1000);
  Serial.print("ph: ");Serial.print(soil_ph);Serial.println(" ph");
  delay(1000);
  Serial.print("Nitrogen: "); Serial.print(val5);Serial.println(" mg/kg");
  delay(1000);
  Serial.print("Phosphorous: ");Serial.print(val6);Serial.println(" mg/kg");
  delay(1000);
  Serial.print("Potassium: ");Serial.print(val7);Serial.println(" mg/kg");
  Serial.println();
  delay(3000);
}

byte moisture() {
  // clear the receive buffer
  mod.flush();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(mois); i++) mod.write(mois[i]);

  // wait for the transmission to complete
  mod.flush();

  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}

byte temperature() {
  // clear the receive buffer
  mod.flush();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(temp); i++) mod.write(temp[i]);

  // wait for the transmission to complete
  mod.flush();

  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[3]<<8|values[4];
}

byte econduc() {
  // clear the receive buffer
  mod.flush();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(econ); i++) mod.write(econ[i]);

  // wait for the transmission to complete
  mod.flush();

  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}

byte phydrogen() {
  // clear the receive buffer
  mod.flush();
  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(ph); i++) mod.write(ph[i]);

  // wait for the transmission to complete
  mod.flush();

  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}

byte nitrogen() {
  // clear the receive buffer
  mod.flush();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(nitro); i++) mod.write(nitro[i]);

  // wait for the transmission to complete
  mod.flush();

  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}

byte phosphorous() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(phos); i++) mod.write(phos[i]);
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  // delay to allow response bytes to be received!
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}

byte potassium() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(pota); i++) mod.write(pota[i]);
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  // delay to allow response bytes to be received!
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    // Serial.print(values[i], HEX);
    // Serial.print(' ');
  }
  return values[4];
}
