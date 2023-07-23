#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP280_ADDRESS 0x76

Adafruit_BMP280 bmp;
#define DHTPIN 4
#define DHTTYPE DHT11

#define COV_RATIO       0.2            // ug/mmm / mv
#define NO_DUST_VOLTAGE 400            // mv
#define SYS_VOLTAGE     5000

int mq135 = A5;
int mq7 = A1;
int mq4 = A2;

int mqData[8];

const int iled = 7;  // Drive the LED of the sensor
const int vout = A0;
float density, voltage;
int adcvalue;
int c1 = 40;
int c2 = 41;
int aq1 = 42;
int aq2 = 43;
int aq3 = 44;
int aq4 = 45;
int aq5 = 46;
int s1 = 47;
int s2 = 48;

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial espSerial(15, 14);  // RX, TX

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  dht.begin();
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW);
  for (int i = 40; i <= 48; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  /*if (!bmp.begin(0x76)) { // Initialize the BMP280 sensor
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }*/
}

void loop() {
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);

  adcvalue = Filter(adcvalue);

  /*
    Convert voltage (mv)
  */
  float voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;

  /*
    Voltage to density
  */
  float density;
  if (voltage >= NO_DUST_VOLTAGE) {
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;
  } else {
    density = 0;
  }

  int mq135Value = analogRead(mq135);
  Serial.print("MQ135 : ");
  Serial.println(mq135Value);
  int mq7Value = analogRead(mq7);
  Serial.print("MQ7 : ");
  Serial.println(mq7Value);
  int mq4Value = analogRead(mq4);
  Serial.print("MQ4 : ");
  Serial.println(mq4Value);
  int humidity = dht.readHumidity();
  Serial.print("Hum : ");
  Serial.println(humidity);
  int temperature = dht.readTemperature();
  Serial.print("Temp : ");
  Serial.println(temperature);
  float pressure = bmp.readPressure();

  if (mq135Value > 300) {
    digitalWrite(c1, HIGH);
    digitalWrite(c2, LOW);
  } 
  else {
    digitalWrite(c1, LOW);
    digitalWrite(c2, HIGH);
  }

  String outputString = String(mq135Value) + "," + String(mq7Value) + "," + String(mq4Value) +
                       "," + String(humidity) + "," + String(temperature) + "," + String(density);
 

  // Send data string to ESP8266
  espSerial.println(outputString);
  Serial.println(outputString);
  

  if (density <= 36) {
    digitalWrite(aq1, HIGH);
    digitalWrite(aq2, LOW);
    digitalWrite(aq3, LOW);
    digitalWrite(aq4, LOW);
    digitalWrite(aq5, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
  } else if (37 <= density && density <= 76) {
    digitalWrite(aq1, LOW);
    digitalWrite(aq2, HIGH);
    digitalWrite(aq3, LOW);
    digitalWrite(aq4, LOW);
    digitalWrite(aq5, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
  } else if (77 <= density && density <= 116) {
    digitalWrite(aq1, LOW);
    digitalWrite(aq2, LOW);
    digitalWrite(aq3, HIGH);
    digitalWrite(aq4, LOW);
    digitalWrite(aq5, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
  } else if (117 <= density && density <= 151) {
    digitalWrite(aq1, LOW);
    digitalWrite(aq2, LOW);
    digitalWrite(aq3, LOW);
    digitalWrite(aq4, HIGH);
    digitalWrite(aq5, LOW);
    digitalWrite(s1, LOW);
    digitalWrite(s2, HIGH);
  } else {
    digitalWrite(aq1, LOW);
    digitalWrite(aq2, LOW);
    digitalWrite(aq3, LOW);
    digitalWrite(aq4, LOW);
    digitalWrite(aq5, HIGH);
    digitalWrite(s1, LOW);
    digitalWrite(s2, HIGH);
  }

  delay(2000);
}

void sendArrayToESP8266(int data[]) {
  for (int i = 0; i < 6; i++) {
    espSerial.println(data[i]);
  }
  delay(2000);
}

int Filter(int m) {
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;

  if (flag_first == 0) {
    flag_first = 1;
    for (i = 0, sum = 0; i < _buff_max; i++) {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  } else {
    sum -= _buff[0];
    for (i = 0; i < (_buff_max - 1); i++) {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    i = sum / 10.0;
    return i;
  }
}
