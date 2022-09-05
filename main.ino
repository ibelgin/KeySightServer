

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "CO2Sensor.h"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define FIREBASE_HOST "https://carbon-aae94-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "Cagl4VeMvze6eX11G5f9hS0M62bbFhSxIRQV2wQm"
#define WIFI_SSID "G-NET"
#define WIFI_PASSWORD "GIRI2004"

//Week Days
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


CO2Sensor co2Sensor(A0, 0.99, 100);
// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;

// Declare global variable to store value



void setup() {

  Serial.begin(115200);// Select the same baud rate if you want to see the datas on Serial Monitor

  //Serial.begin(9600);
  Serial.println("=== Initialized ===");
  co2Sensor.calibrate();

  Serial.println("Serial communication started\n\n");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);



  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(19800);

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase

  Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop() {

  // Firebase Error Handling And Writing Data At Specifed Path************************************************
  int val = co2Sensor.read();
  Serial.print("CO2 value: ");
  Serial.println(val);
  //delay(1000);


  if (Firebase.setInt(firebaseData, "/data", val)) {    // On successful Write operation, function returns 1
    Serial.println("Value Uploaded Successfully");
    val++;
    delay(1000);

  }

  else {
    Serial.println(firebaseData.errorReason());
  }
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();


  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);

  int currentHour = timeClient.getHours();

  int currentMinute = timeClient.getMinutes();


  int currentSecond = timeClient.getSeconds();

  String weekDay = weekDays[timeClient.getDay()];

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  int monthDay = ptm->tm_mday;


  int currentMonth = ptm->tm_mon + 1;


  String currentMonthName = months[currentMonth - 1];


  int currentYear = ptm->tm_year + 1900;

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");

  delay(1000);
}
