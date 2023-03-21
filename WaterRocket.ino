// Made and Licensed by BudgetArms
// 21 March 2023

// Step One

// Install these via Library Manager ( Sketch < Include Libary < Manage Library ) : 
// Adafruit BMP280 bye Adafruit

// if you are installing the library for the first time you should get a popup.
// Asking to download the other library dependencies Adafruit Unified Sensor and Adafruit BusIO.
// Press installl all.

// if you DONT get the pop-up, download these libraries :
// Adafruit Unified Sensor by Adafruit
// AdafruitBusIO by Adafruit



// Step Two

// Go to File > Preferences
// Then paste the link(s) below into the "Additional Board Manager URLs" field.


// If you have ESP32 Board then paste this link:
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

// Then go to Tools > Board > Boards Manager,
// and search for ESP32 (by Espressif Systems) and Install it.

// Then go to Tools > Board menu and select your type of board, (under ESP32 Boards) a commonly used board is the DOIT ESP32 DEVKIT V1 . 


// If you have ESP8366 Board then paste these links:
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json

// Then go to Tools > Board > Boards Manager :
// And search for ESP8266 (by ESP8266 Community)and Install it.

 // Then go to Tools > Board and select your type of board (under ESP8266 Boards), a commonly used board is the NodeMCU 1.0 (ESP-12E Module) or the NodeMCU 0.9 (ESP-12 Module).

 // I used the NodeMCU 1.0 (ESP-12E Module) or the NodeMCU 0.9 (ESP-12Module)


// Step Three

// Check if the Upload speed is set to 115200 (default for ESP32/ESP8266) and the same in your Serial.begin(115200).
// After that select the port to your ESP32/ESP8266 ( COM4 / COM5 ) mostely used.


// Step Four

// Download these zip files by click these link, they include the ESPAsyncWebServer and the ESPAsyncTCP library.
// https://codeload.github.com/me-no-dev/ESPAsyncWebServer/zip/refs/heads/master
// https://codeload.github.com/me-no-dev/ESPAsyncTCP/zip/refs/heads/master

// To include these libraries to the Arduino IDE (the Arduino Software),
// Go to Sketch < Include Libary < Add .ZIP Library.
// And select the ESPAsyncWebServer-master.zip and press open
// Do the same thing for the ESPAsyncTCP-master.zip folder.


// Step Five
// Run the Code and make a commit if you got something new to add or to report a bug.



// Import the required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "MPU9250.h"
    


// MPU9250 is an object of the class MPU9250
MPU9250 mpu = MPU9250();


Servo servo;
Adafruit_BMP280 bmp; // I2C


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


// WiFi Name
const char* WiFi_SSID {"WiFi Name"};
const char* WiFi_Password {"WiFi Password"};
// Check you WiFI

// Variables
bool Ready{};
const int servoPin{ D8 };
int servoRotation{ 130 };
bool parachuteOpen{};


float accelgZ{};
float altitude{};


float pressureOnGround{};
int nrCyclesGroundPreassure{ 10 };


const int arrSize{ 20 };
float arrAltitude[arrSize]{};


int descendingCounter{};

// I made this to give time to the sensors 
// to start up, check it you can set it to zero
// and check if the sensors then still work correctly
const int firstLoopDelay = 45000;

// This is so that the sensors dont crash because they get 
// fired too often.
// This should probably be higher for your sensors.
// Test it out to be sure!
const int loopDelay = 100;

bool InitializeSensors();
void readGroundLevel();
void CreateServerRequests(AsyncWebServer server);


void TransformArray();
void DisplayArray();
void ArraySorter();
void WriteToFile();
void OpenServo();


String IsRocketReady();
String IsParachuteOpen();

String readBMP280Altiude();
String readMPU280AccelgZ();
String readBMP280Temperature();



void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  

  if(!InitializeSensors())
  {
    //There is something so wrong with the sensors
    //so we "stop" the program by going into an infinite loop
    Serial.println("Program stoped!");

    while(1);
  }

  
  // Defining Sealevel Pressure
  readGroundLevel();
  

  Serial.print("Setting AP (Access Point)…");
  // Remove the WiFi_Password parameter, if you want the AP (Access Point) to be open

  WiFi.softAP(WiFi_SSID, "");
  //WiFi.softAP(WiFi_SSID, WiFi_Password);

  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);


  // If the function below does not work.
  // Replace the function with the code 
  // inside the function
  CreateServerRequests(server);

  // Start the server
  server.begin();

}

void loop()
{

  // Wait until the system started!
  if(!Ready)
  {
    delay(firstLoopDelay);
    Ready = true;

    Serial.println("READY TO LAUNCH!!!");
  }  


  altitude = bmp.readAltitude(pressureOnGround);

  TransformArray();
  //Uncomment the DisplayArray to debug the data on the ground (before launching)
  //DisplayArray();
  ArraySorter();
  
  WriteToFile();

  delay(loopDelay);
}



bool  InitializeSensors()
{
  
  // Intialize the MPU9250 sensor.
  // This is done in the beginning because
  // otherwise the MPU does not work like it should.
  uint8_t temp = mpu.begin();

  bool statusSensors{};

  statusSensors = mpu.begin();
  if (statusSensors < 0) 
  {
    Serial.println("Could not find a valid MPU9250 sensor, check wiring!");
    return 0;
  }


  // enable the MPU to meassure up to 16G
  mpu.set_accel_range(RANGE_16G);

  // This 0x76 can be something else depending on the hardware
  statusSensors = bmp.begin(0x76);  


  if (!statusSensors) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    return 0;
  }


  // Initialize SPIFFS (the file system)
  if(!SPIFFS.begin())
  {
    Serial.println("ERROR::An Error has occurred while mounting SPIFFS");
    Serial.println("Program stoped!");
    while(1);
  }

  return 1;
}

void readGroundLevel() 
{
  Serial.println("Meassuring GroundPreassure...");
  Serial.println("This takes about 10 seconds");
  
  float sum{};
  float averagePressure{};
  
  for(int i = 0; i < nrCyclesGroundPreassure; i++) 
  {
    delay(1000);
    sum+=(bmp.readPressure() / 100.0F);
  }

  averagePressure = sum/nrCyclesGroundPreassure;
  pressureOnGround = averagePressure;

  Serial.print("Ground Preassure: ");
  Serial.println(pressureOnGround);
}

void CreateServerRequests(AsyncWebServer server)
{

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html");
  });
  server.on("/highcharts.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/highcharts.js");
  });
  server.on("/Script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/Script.js");
  });
  server.on("/Style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/Style.css");
  });
  server.on("/favicon.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/favicon.jpg");
  });
  server.on("/tempfile.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/tempfile.html");
  });

  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/plain", readBMP280Altiude().c_str());
  });
  server.on("/accelerationZ", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send_P(200, "text/plain", readMPU280AccelgZ().c_str());
  });
  
  server.on("/Ready", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send_P(200, "text/plain", IsRocketReady().c_str());
  });
  
  server.on("/parachuteOpen", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send_P(200, "text/plain", IsParachuteOpen().c_str());
  }); 

}


void TransformArray()
{
  for(int i = (arrSize-1); i > 0; i--)
  {
    arrAltitude[i] = arrAltitude[i-1];
  } 

  arrAltitude[0] = altitude;
}

void DisplayArray()
{
  for(int i = 0; i < arrSize; i++) 
  {
    Serial.print(arrAltitude[i]);
    Serial.print("\t"); 
  }
  
  Serial.println("");
  delay(100);
}

void ArraySorter() 
{

  // If the difference between the previous meassured altitude and the current altitude is bigger than 0.5 meters
  // Then increment the variable 'descendingCounter' by one.
  // Else set the variable 'descendingCounter' to zero.
  if( 0.5 <= (arrAltitude[1] - arrAltitude[0])) 
  {
    ++descendingCounter; // or descendingCounter++ or descendingCounter = descendingCounter + 1 (same thing)
  } 
  else 
  {
    descendingCounter = 0; // reset the counter
  }

  // If the variable 'descendingCounter' is equal to three or higher and 
  // the parachute is not open then run the function OpenServo.
  if((3 <= descendingCounter) && !parachuteOpen)
  {
    OpenServo();
  }
}

void WriteToFile()
{
  File tempfile = SPIFFS.open("/tempfile.html", "a");

  if(tempfile)
  {
    int bytesWritten = tempfile.println(readBMP280Altiude().c_str());

    if(bytesWritten == 0) 
    {
      Serial.println("ERROR::File Write Failed");
    }

    bytesWritten = tempfile.println("&nbsp;&nbsp;&nbsp;&nbsp;");
    bytesWritten = tempfile.println(readMPU280AccelgZ().c_str());

    if(bytesWritten == 0) 
    {
      Serial.println("ERROR::File Write Failed");
    }

    bytesWritten = tempfile.println("<br>");
    tempfile.close();
  }
  else
  {
    Serial.println("ERROR::File could not be opened");
    Serial.println("Probably due to the file having another name");
    Serial.println("Or the file is opening in another program");
    return;
  }
  
}

void OpenServo()
{
  Serial.println("Servo fired");
  parachuteOpen = true;
  

  servo.attach(servoPin);

  // This might need to bechange for your servo
  // Test the code to be sure.
  servo.write(0); // rotate the servo motor AGAIN by 90 degree

  // My servo is glitched so check if
  // the degrees are correct for your servo
  servoRotation = 2.94444444444f * servoRotation; 
 
  delay(servoRotation); // Also check this with your own servo motor

  servo.detach();
}



String IsRocketReady()
{
  return String(Ready);
}

String IsParachuteOpen()
{
  return String(parachuteOpen);
}


String readBMP280Altiude() 
{
  altitude = bmp.readAltitude(pressureOnGround);

  if (!isnan(altitude)) 
  {
    return String(altitude);
  } 

  Serial.println("ERROR::Failed to read 'altitude' from BMP280 sensor!"); 
  return "";
  
}

String readMPU280AccelgZ() 
{
  mpu.get_accel_g();
  accelgZ = mpu.z_g;

  if (!isnan(accelgZ)) 
  {
    return String(accelgZ);
  }
  
  Serial.println("Failed to read from MPU9250 sensor!");
  return "";

}

String readBMP280Temperature() 
{
  float temperature = bmp.readTemperature();
  
  if (!isnan(temperature)) {    
    return String(temperature);
  }

  Serial.println("ERROR::Failed to read 'temperature' from BME280 sensor!");
  return "";
}
