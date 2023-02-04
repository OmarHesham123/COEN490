/* part of code used from https://randomnerdtutorials.com/esp32-firebase-realtime-database/#create-firebase-project*/


#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>  // connects the ESP32 to the internet
#endif
#include <Firebase_ESP_Client.h>  //connects from the board to the Firebase
#include <Wire.h>

//include sensor libraries
#include <Arduino_BMI270_BMM150.h>      //connects to motion sensor
#include <SparkFun_TMP117.h>            //connects to temp sensor
#include <SparkFun_TMP117_Registers.h>  //temp sensor
#include <MAX30105.h>                   //sensor MAX30101 (same library)
#include <heartRate.h>                  //sensor MAX30101 bmp
#include <spo2_algorithm.h>             //sensor ox pulse MAX30101

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ISpySomethingBlue"  //hardcoded wifi...  need to link to the phone wifi... 
#define WIFI_PASSWORD "network_password". //change to the password

// Insert Firebase project API Key
#define API_KEY "AIzaSyCw6R30Xwdptj30RgnouxmqdeQ_Diy0EiU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://coen-490-45c88-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int count = 0;
bool signupOK = false;

//variables to save to database paths
String databasePath;
String tempPath;
String bpmPath;
String oxPath;
String motionPath;

//Timer variables (send new readings every 30 seconds)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 30000;

//BMI270 sensor
Arduino_BMI270_BMM150 bmi;
float acceleration;
float gyroscope;
float

//TMP117 sensor
SparkFun_TMP117 tmp;
float temperature;


//MAX30101 sensor
MAX30105 max;
float bpm;
float oxmeter;



//initialize BMI270
void initBMI() {
  if (!bmi.begin(...)) {. //replace ... with register address
    Serial.println("Could not find a valid BMI270 sensor, check wiring..");
    while (1);
  }
}
  //initialize TMP117
void initTMP() {
  if(!tmp.begin(...)) { //replace ... with register address
    Serial.println("Could not find valid TMP117 sensor, check wiring..");
while (1);    
  }
}
  //initialize MAX30101
void initMAX() {
  if(!tmp.begin(...)) { //replace ... with register address
    Serial.println("Could not find valid MAX30101 sensor, check wiring..");
while (1);    
  }
}

  //initialize WiFi
  void
  initWiFi() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100))) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}