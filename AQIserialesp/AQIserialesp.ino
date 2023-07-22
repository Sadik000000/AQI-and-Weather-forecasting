#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
int val1 ;
int da1 ;
int val2 ;
int da2 ;
int val3 ;
int da3 ;
int val4 ;
int da4 ;
int val5 ;
int da5 ;
int val6 ;
int da6 ;
int val7 ;
int da7 ;
int rainPin = A0;

// Firebase credentials
#define FIREBASE_HOST "https://wfaqi-f7978-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "qzoCc6jsXpYis0elx2CRRdfalMMWPDyF2awrCWCZ"

// WiFi credentials
#define WIFI_SSID "BB"
#define WIFI_PASSWORD "12345123"

// Arduino serial communication
#define ARDUINO_SERIAL Serial

FirebaseData firebaseData;
FirebaseJson json;

void setup() {
  Serial.begin(9600);
  ARDUINO_SERIAL.begin(9600);
  

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(rainPin, INPUT);
}

void loop() {
  if (ARDUINO_SERIAL.available()) {
    String data1 = ARDUINO_SERIAL.readStringUntil('\n');
    String data2 = ARDUINO_SERIAL.readStringUntil('\n');
    String data3 = ARDUINO_SERIAL.readStringUntil('\n');
    String data4 = ARDUINO_SERIAL.readStringUntil('\n');
    String data5 = ARDUINO_SERIAL.readStringUntil('\n');
    String data6 = ARDUINO_SERIAL.readStringUntil('\n');
    String data7 = ARDUINO_SERIAL.readStringUntil('\n');
    Serial.println(data1);
    Serial.println(data2);
    Serial.println(data3);
    Serial.println(data4);
    Serial.println(data5);
    Serial.println(data6);
    Serial.println(data7);
    val1 = data1.toInt();
    val2 = data2.toInt();
    val3 = data3.toInt();
    val4 = data4.toInt();
    val5 = data5.toInt();
    val6 = data6.toInt();
    val7 = data7.toInt();

    int sensorValue = analogRead(rainPin);  // Read the sensor value

/*json.set("/C", String(da1));
  Firebase.updateNode(firebaseData, "/sensorData", json);*/
  

    if (Firebase.setInt(firebaseData, "/CO2", da1 ,firebaseData )) {
  da1 = val1;
  }
  if (Firebase.setInt(firebaseData, "/CO", da2 , firebaseData)) {
  da2 = val2;
  }
  if (Firebase.setInt(firebaseData, "/Methane", da3 , firebaseData)) {
  da3 = val3;
  } 
  if (Firebase.setInt(firebaseData, "/Humidity", da4, firebaseData)) {
  da4 = val4;
  }
  if (Firebase.setInt(firebaseData, "/Temperature", da5 ,firebaseData)) {
  da5 = val5;
  }
  if (Firebase.setInt(firebaseData, "/Pressure", da6, firebaseData)) {
  da6 = val6;
  }
  if (Firebase.setInt(firebaseData, "/UV_INTENSITY", da7 ,firebaseData)) {
  da7 = val7;
  }  
  
  else 
    {

      Serial.println("Error sending data to Firebase");
      Serial.println(firebaseData.errorReason());
    }
    
    if (val5 > 35) {
    //Send an alert to Firebase
    Firebase.setString(firebaseData, "/alert1", "High temperature detected!");
  } 
  else {
    //Clear the alert in Firebase
    Firebase.setString(firebaseData, "/alert1", "");
  }

if (val1 > 300) {
    // Send an alert to Firebase
    Firebase.setString(firebaseData, "/alert2", "High CO2 detected!");
  } else {
    // Clear the alert in Firebase
    Firebase.setString(firebaseData, "/alert2", "");
  }

if (sensorValue < 500) {
    Firebase.setString(firebaseData, "/alert3", "Rain detected!");
   Serial.println("Rain detected!");
  } else {
    Firebase.setString(firebaseData, "/alert3", "");
    Serial.println("No rain detected.");
  }
  delay(200);

  }
}
