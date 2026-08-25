/*
=========================================================
 AEROROVER - DRONE INSPECTION MODULE
 File: AeroRover_Drone.ino

 Features:
 - Temperature
 - Humidity
 - Pressure
 - Estimated Altitude
 - Rain / Moisture
 - Heat Detection
 - Gas Monitoring
 - GPS Live Position
 - MPU6050 Orientation
 - Pitch and Roll
 - Battery Monitoring
 - AI Detection Placeholder
 - Drone Web Dashboard
=========================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

#include <Adafruit_BME280.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <TinyGPSPlus.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// =====================================================
// I2C
// =====================================================

#define I2C_SDA 1
#define I2C_SCL 2

// =====================================================
// ANALOG PINS
// =====================================================

#define MOISTURE_PIN 4
#define GAS_PIN 5
#define BATTERY_PIN 6

// =====================================================
// GPS
// =====================================================

#define GPS_RX 17
#define GPS_TX 18

HardwareSerial GPSSerial(1);

TinyGPSPlus gps;

// =====================================================
// SENSOR OBJECTS
// =====================================================

Adafruit_BME280 bme;

Adafruit_MLX90614 mlx =
Adafruit_MLX90614();

Adafruit_MPU6050 mpu;

// =====================================================
// SENSOR VALUES
// =====================================================

float temperature = 0;
float humidity = 0;
float pressure = 0;

float heat = 0;

int moisture = 0;
int gasValue = 0;

float latitude = 0;
float longitude = 0;

float estimatedAltitude = 0;

float accelerationX = 0;
float accelerationY = 0;
float accelerationZ = 0;

float pitch = 0;
float roll = 0;

float batteryVoltage = 0;
int batteryPercent = 0;

// =====================================================
// FLIGHT TELEMETRY
// Replace with real MAVLink / Flight Controller data
// =====================================================

float flightAltitude = 0;
float flightSpeed = 0;

String flightMode = "STANDBY";

// =====================================================
// AI VARIABLES
// =====================================================

String detectedObject = "Scanning";
String threatLevel = "NORMAL";

int aiConfidence = 0;

// =====================================================
// DASHBOARD
// =====================================================

const char DRONE_PAGE[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>AeroRover Drone</title>

<style>

body{
margin:0;
padding:20px;
font-family:Arial,sans-serif;
background:#111827;
color:white;
}

h1{
font-size:32px;
color:#60a5fa;
}

.grid{
display:grid;
grid-template-columns:
repeat(auto-fit,minmax(210px,1fr));
gap:15px;
}

.card{
background:#1f2937;
border:1px solid #374151;
border-radius:12px;
padding:16px;
}

.label{
color:#9ca3af;
font-size:15px;
}

.value{
color:#22c55e;
font-size:24px;
font-weight:bold;
margin-top:8px;
}

.alert{
color:#facc15;
}

</style>

</head>

<body>

<h1>🚁 AeroRover Drone Inspection System</h1>

<p>
Live Aerial Monitoring | AI Detection | GPS | Emergency Response
</p>

<div class="grid">

<div class="card">
<div class="label">Temperature</div>
<div class="value" id="temp">-- °C</div>
</div>

<div class="card">
<div class="label">Humidity</div>
<div class="value" id="humidity">-- %</div>
</div>

<div class="card">
<div class="label">Pressure</div>
<div class="value" id="pressure">-- hPa</div>
</div>

<div class="card">
<div class="label">Estimated Altitude</div>
<div class="value" id="altitude">-- m</div>
</div>

<div class="card">
<div class="label">Rain / Moisture</div>
<div class="value" id="moisture">-- %</div>
</div>

<div class="card">
<div class="label">Heat / Hotspot</div>
<div class="value" id="heat">-- °C</div>
</div>

<div class="card">
<div class="label">Gas / Air Quality</div>
<div class="value" id="gas">--</div>
</div>

<div class="card">
<div class="label">Battery</div>
<div class="value" id="battery">-- %</div>
</div>

<div class="card">
<div class="label">GPS Position</div>
<div class="value" id="gps">Waiting...</div>
</div>

<div class="card">
<div class="label">Pitch / Roll</div>
<div class="value" id="orientation">-- °</div>
</div>

<div class="card">
<div class="label">Flight Mode</div>
<div class="value" id="flightMode">STANDBY</div>
</div>

<div class="card">
<div class="label">AI Detection</div>
<div class="value alert" id="ai">Scanning</div>
</div>

</div>

<script>

function updateDrone(){

fetch("/telemetry")

.then(response => response.json())

.then(data => {

document.getElementById("temp").innerHTML =
data.temperature + " °C";

document.getElementById("humidity").innerHTML =
data.humidity + " %";

document.getElementById("pressure").innerHTML =
data.pressure + " hPa";

document.getElementById("altitude").innerHTML =
data.altitude + " m";

document.getElementById("moisture").innerHTML =
data.moisture + " %";

document.getElementById("heat").innerHTML =
data.heat + " °C";

document.getElementById("gas").innerHTML =
data.gas;

document.getElementById("battery").innerHTML =
data.battery + " %";

document.getElementById("gps").innerHTML =
data.latitude.toFixed(6) +
"<br>" +
data.longitude.toFixed(6);

document.getElementById("orientation").innerHTML =
data.pitch + "° / " +
data.roll + "°";

document.getElementById("flightMode").innerHTML =
data.flightMode;

document.getElementById("ai").innerHTML =
data.detectedObject +
"<br>" +
data.aiConfidence + "%";

});

}

setInterval(
updateDrone,
1000
);

updateDrone();

</script>

</body>
</html>

)rawliteral";

// =====================================================
// GPS
// =====================================================

void updateGPS(){

while(GPSSerial.available()){

gps.encode(
GPSSerial.read()
);

}

if(gps.location.isValid()){

latitude =
gps.location.lat();

longitude =
gps.location.lng();

}

}

// =====================================================
// SENSOR UPDATE
// =====================================================

void updateSensors(){

// -------------------------
// BME280
// -------------------------

temperature =
bme.readTemperature();

humidity =
bme.readHumidity();

pressure =
bme.readPressure() / 100.0F;

// Estimated altitude from pressure

estimatedAltitude =
bme.readAltitude(1013.25);

// -------------------------
// MLX90614
// -------------------------

heat =
mlx.readObjectTempC();

// -------------------------
// Moisture / Rain
// -------------------------

int moistureRaw =
analogRead(
MOISTURE_PIN
);

moisture =
map(
moistureRaw,
0,
4095,
100,
0
);

moisture =
constrain(
moisture,
0,
100
);

// -------------------------
// Gas
// -------------------------

gasValue =
analogRead(
GAS_PIN
);

// -------------------------
// MPU6050
// -------------------------

sensors_event_t a;
sensors_event_t g;
sensors_event_t tempEvent;

mpu.getEvent(
&a,
&g,
&tempEvent
);

accelerationX =
a.acceleration.x;

accelerationY =
a.acceleration.y;

accelerationZ =
a.acceleration.z;

roll =
atan2(
accelerationY,
accelerationZ
) * 180 / PI;

pitch =
atan2(
-accelerationX,
sqrt(
accelerationY *
accelerationY +

accelerationZ *
accelerationZ
)
) * 180 / PI;

// -------------------------
// Battery
// -------------------------

int batteryRaw =
analogRead(
BATTERY_PIN
);

// Calibrate according
// to your voltage divider.

batteryVoltage =
(
batteryRaw / 4095.0
) * 3.3 * 2.0;

batteryPercent =
map(
(int)(batteryVoltage * 100),
330,
420,
0,
100
);

batteryPercent =
constrain(
batteryPercent,
0,
100
);

}

// =====================================================
// FLIGHT DATA
// =====================================================

void updateFlightTelemetry(){

/*

For the final version:

Get these values from
Pixhawk / ArduPilot /
PX4 flight controller.

Possible data:

GPS
Altitude
Speed
Flight Mode
Battery
Yaw

Communication:
UART + MAVLink

For now the altitude
uses BME280 estimation.

*/

flightAltitude =
estimatedAltitude;

flightSpeed =
0;

flightMode =
"INSPECTION";

}

// =====================================================
// AI
// =====================================================

void runDroneAI(){

/*

Future AI Pipeline:

DRONE CAMERA
      ↓
IMAGE FRAME
      ↓
EDGE IMPULSE / TFLITE
      ↓
Detection

Classes:

Person
Victim
Fire
Smoke
Damaged Building
Vehicle
Hazard Zone

*/

if(heat > 60){

detectedObject =
"Possible Heat / Fire Source";

aiConfidence = 88;

threatLevel =
"HIGH";

}else{

detectedObject =
"Area Scanning";

aiConfidence = 94;

threatLevel =
"NORMAL";

}

}

// =====================================================
// WEB HANDLERS
// =====================================================

void handleRoot(){

server.send(
200,
"text/html",
DRONE_PAGE
);

}

void handleTelemetry(){

updateGPS();

updateSensors();

updateFlightTelemetry();

runDroneAI();

String json = "{";

json +=
"\"device\":\"DRONE_01\",";

json +=
"\"temperature\":" +
String(temperature,1) + ",";

json +=
"\"humidity\":" +
String(humidity,1) + ",";

json +=
"\"pressure\":" +
String(pressure,1) + ",";

json +=
"\"altitude\":" +
String(flightAltitude,1) + ",";

json +=
"\"moisture\":" +
String(moisture) + ",";

json +=
"\"heat\":" +
String(heat,1) + ",";

json +=
"\"gas\":" +
String(gasValue) + ",";

json +=
"\"battery\":" +
String(batteryPercent) + ",";

json +=
"\"latitude\":" +
String(latitude,6) + ",";

json +=
"\"longitude\":" +
String(longitude,6) + ",";

json +=
"\"pitch\":" +
String(pitch,1) + ",";

json +=
"\"roll\":" +
String(roll,1) + ",";

json +=
"\"flightMode\":\"" +
flightMode + "\",";

json +=
"\"detectedObject\":\"" +
detectedObject + "\",";

json +=
"\"aiConfidence\":" +
String(aiConfidence) + ",";

json +=
"\"threatLevel\":\"" +
threatLevel + "\"";

json += "}";

server.send(
200,
"application/json",
json
);

}

// =====================================================
// SETUP
// =====================================================

void setup(){

Serial.begin(115200);

// I2C

Wire.begin(
I2C_SDA,
I2C_SCL
);

// GPS

GPSSerial.begin(
9600,
SERIAL_8N1,
GPS_RX,
GPS_TX
);

// Analog Inputs

pinMode(
MOISTURE_PIN,
INPUT
);

pinMode(
GAS_PIN,
INPUT
);

pinMode(
BATTERY_PIN,
INPUT
);

// BME280

if(!bme.begin(0x76)){

Serial.println(
"BME280 not found!"
);

}

// MLX90614

if(!mlx.begin()){

Serial.println(
"MLX90614 not found!"
);

}

// MPU6050

if(!mpu.begin()){

Serial.println(
"MPU6050 not found!"
);

}

// WiFi

WiFi.begin(
ssid,
password
);

Serial.print(
"Connecting Drone WiFi"
);

while(
WiFi.status() != WL_CONNECTED
){

delay(500);

Serial.print(".");

}

Serial.println();

Serial.print(
"Drone IP Address: "
);

Serial.println(
WiFi.localIP()
);

// Web Server Routes

server.on(
"/",
handleRoot
);

server.on(
"/telemetry",
handleTelemetry
);

server.begin();

Serial.println(
"Drone Server Started"
);

}

// =====================================================
// LOOP
// =====================================================

void loop(){

server.handleClient();

updateGPS();

}