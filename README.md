# AeroRover---Drone-Deployed-Multi-Robot-System-for-Remote-Inspection-and-Emergency-Response
# 🚁 AeroRover

### Drone-Deployed Multi-Robot System for Remote Inspection and Emergency Response

> **AeroRover** is a modular robotic system that combines an aerial drone and a deployable ground rover to perform remote inspection, environmental sensing, obstacle detection, and emergency-response operations in areas that are difficult or unsafe for humans to access.

---

## 📌 Overview

AeroRover combines:

* 🚁 **Autonomous/remote inspection drone**
* 🤖 **Ground inspection rover**
* 📡 **Wireless sensor communication**
* 🧠 **XIAO ESP32S3 Sense-based edge processing**
* 📍 **GPS-based location tracking**
* 📏 **LiDAR-based obstacle/distance measurement**
* 🌡️ **Environmental monitoring**
* 🧪 **Gas detection**
* 📳 **Vibration and motion monitoring**
* 🎥 **Camera-based visual inspection**
* 🌐 **Real-time web dashboard**

The drone is designed to reach and survey an area from the air, while the rover can be deployed to the ground for closer inspection.

---

# 🏗️ System Architecture

```text
                         ┌─────────────────────────┐
                         │     AeroRover System    │
                         └────────────┬────────────┘
                                      │
                     ┌────────────────┴────────────────┐
                     │                                 │
                     ▼                                 ▼
              🚁 AERIAL UNIT                     🤖 GROUND UNIT
                 DRONE                              ROVER
                     │                                 │
              XIAO ESP32S3                       XIAO ESP32S3
                     │                                 │
        ┌────────────┼────────────┐       ┌────────────┼────────────┐
        │            │            │       │            │            │
       GPS         LiDAR        MPU6050  Gas         DHT22       MPU6050
        │            │            │       │            │            │
        └────────────┴────────────┘       └────────────┴────────────┘
                     │                                 │
                     └──────────── Wi-Fi ──────────────┘
                                      │
                                      ▼
                           🌐 Web Monitoring System
```

---

# ✨ Key Features

## 🚁 Drone

The drone provides aerial reconnaissance and location awareness.

### Sensors

| Component          | Purpose                                |
| ------------------ | -------------------------------------- |
| XIAO ESP32S3 Sense | Edge controller / sensor gateway       |
| GPS                | Position and altitude information      |
| VL53L0X LiDAR      | Distance and obstacle detection        |
| MPU6050            | Acceleration and gyroscope data        |
| BME280             | Temperature, humidity and pressure     |
| Flight Controller  | Flight stabilization and motor control |
| ESCs               | Brushless motor control                |
| Brushless Motors   | Propulsion                             |
| RC Receiver        | Manual/remote control                  |

### Drone telemetry

The web interface can display:

* Latitude
* Longitude
* Altitude
* Satellite count
* LiDAR distance
* Temperature
* Humidity
* Pressure
* Accelerometer values
* Gyroscope values
* Wi-Fi RSSI
* System uptime

---

# 🤖 Rover

The rover provides ground-level inspection after the drone identifies an area of interest.

### Sensors

| Component          | Purpose                              |
| ------------------ | ------------------------------------ |
| XIAO ESP32S3 Sense | Rover controller / sensor gateway    |
| MQ-2 / MQ-135      | Gas/environmental detection          |
| DHT22              | Temperature and humidity             |
| SW-420             | Vibration detection                  |
| MPU6050            | Motion, acceleration and orientation |
| Camera             | Visual inspection                    |
| Motor Driver / ESC | Rover propulsion                     |

### Rover telemetry

The dashboard can display:

* Gas sensor value
* Gas warning status
* Temperature
* Humidity
* Vibration status
* X/Y/Z acceleration
* X/Y/Z gyroscope
* Wi-Fi RSSI
* System uptime

---

# 🔌 Hardware Connections

## 🚁 Drone XIAO ESP32S3 Sense

### I²C Bus

The MPU6050, BME280 and VL53L0X share the same I²C bus.

```text
XIAO ESP32S3
     │
     ├── D5 / SDA ───── MPU6050 SDA
     │              ├── BME280 SDA
     │              └── VL53L0X SDA
     │
     └── D6 / SCL ───── MPU6050 SCL
                    ├── BME280 SCL
                    └── VL53L0X SCL
```

### GPS

```text
GPS TX ───── XIAO GPS RX
GPS RX ───── XIAO GPS TX
GPS GND ──── XIAO GND
GPS VCC ──── Appropriate regulated supply
```

UART pins should be adjusted in the Arduino sketch to match the actual XIAO board pin mapping.

---

# 🤖 Rover XIAO ESP32S3 Sense

| XIAO Pin | Device        | Function                  |
| -------- | ------------- | ------------------------- |
| D1 / A0  | Gas sensor AO | Analog gas reading        |
| D2       | DHT22         | Temperature/Humidity      |
| D3       | SW-420        | Vibration digital signal  |
| D5       | MPU6050 SDA   | I²C data                  |
| D6       | MPU6050 SCL   | I²C clock                 |
| 3.3V     | Sensors       | Low-voltage sensor supply |
| GND      | All modules   | Common ground             |

### Rover I²C

```text
             XIAO ESP32S3
                  │
          ┌───────┴───────┐
          │               │
        D5/SDA          D6/SCL
          │               │
          └───────┬───────┘
                  │
              MPU6050
```

---

# ⚡ Power Architecture

The drone and rover should use separate regulated power systems.

## Drone

```text
4S LiPo Battery
       │
       ▼
Power Distribution / PDB
       │
       ├── ESC 1 ── Motor 1
       ├── ESC 2 ── Motor 2
       ├── ESC 3 ── Motor 3
       ├── ESC 4 ── Motor 4
       │
       └── BEC / Regulator
               │
               ├── Flight Controller
               └── XIAO + Sensors
```

## Rover

```text
Battery
   │
   ▼
Power Distribution
   │
   ├── Motor Driver ── Motors
   │
   └── Buck Converter
           │
           └── XIAO + Sensors
```

### ⚠️ Power Safety

* Do not connect a raw LiPo voltage directly to the XIAO.
* Use an appropriate regulator/BEC.
* Verify the operating voltage of every sensor before connecting it.
* MQ-series gas modules can operate at 5 V and may produce an analog output unsuitable for direct ESP32 ADC input.
* Use an appropriate voltage divider or level-shifting circuit for the gas sensor analog output.
* Maintain a common ground between communicating modules.
* Never connect motor power directly to GPIO pins.

---

# ⚙️ Drone Flight-Control Architecture

The XIAO ESP32S3 Sense is **not intended to replace the dedicated flight controller** in this architecture.

The flight controller handles:

```text
RC Receiver
     │
     ▼
Flight Controller
     │
     ├── ESC 1 ── Motor 1
     ├── ESC 2 ── Motor 2
     ├── ESC 3 ── Motor 3
     └── ESC 4 ── Motor 4
```

The XIAO provides the additional sensing, telemetry and web interface.

```text
                ┌────────────────┐
                │ Flight Control │
                └───────┬────────┘
                        │
                 Flight signals
                        │
                  ┌─────▼─────┐
                  │   Motors  │
                  └───────────┘

                ┌────────────────┐
                │ XIAO ESP32S3  │
                │     Sense      │
                └───────┬────────┘
                        │
              Sensor telemetry
                        │
                        ▼
                  Web Dashboard
```

This separation keeps **flight-critical stabilization** independent from the experimental telemetry system.

---

# 🧰 Software Stack

## Embedded

* Arduino IDE
* ESP32 Arduino Core
* C/C++
* XIAO ESP32S3 Sense

## Libraries

Typical libraries used by the project include:

```text
WiFi
WebServer
Wire
Adafruit MPU6050
Adafruit Unified Sensor
Adafruit BME280
Adafruit VL53L0X
TinyGPSPlus
DHT sensor library
```

---

# 📁 Repository Structure

Recommended GitHub structure:

```text
AeroRover/
│
├── README.md
│
├── drone/
│   ├── AeroRover_Drone.ino
│   ├── sensors/
│   └── wiring/
│       └── drone-wiring-schematic.png
│
├── rover/
│   ├── AeroRover_Rover.ino
│   ├── sensors/
│   └── wiring/
│       └── rover-wiring-schematic.png
│
├── dashboard/
│   ├── index.html
│   ├── css/
│   └── js/
│
├── hardware/
│   ├── drone/
│   ├── rover/
│   └── schematics/
│
├── models/
│   └── README.md
│
├── docs/
│   ├── architecture.md
│   ├── implementation.md
│   └── testing.md
│
├── images/
│   ├── drone.jpg
│   ├── rover.jpg
│   └── system-overview.png
│
└── LICENSE
```

---

# 🚀 Implementation

## Step 1 — Assemble the drone

Connect:

```text
GPS
LiDAR
MPU6050
BME280
XIAO ESP32S3 Sense
```

to the drone electronics.

The flight controller, ESCs, motors and receiver remain responsible for flight operations.

---

## Step 2 — Assemble the rover

Connect:

```text
MQ Gas Sensor
DHT22
SW-420
MPU6050
XIAO ESP32S3 Sense
Motor Driver
Motors
```

The XIAO collects the environmental and motion telemetry.

---

## Step 3 — Install Arduino IDE

Install the ESP32 board package and select the appropriate:

```text
XIAO ESP32S3
```

board configuration.

---

## Step 4 — Install libraries

Install the required libraries through:

```text
Arduino IDE
    ↓
Library Manager
```

Search for:

```text
Adafruit MPU6050
Adafruit Unified Sensor
Adafruit BME280
Adafruit VL53L0X
TinyGPSPlus
DHT sensor library
```

---

# 🌐 Web Server

Both XIAO boards can create their own Wi-Fi access point.

## Drone

```text
SSID:
AeroRover-DRONE

Password:
12345678

IP:
192.168.4.1
```

Open:

```text
http://192.168.4.1
```

## Rover

```text
SSID:
AeroRover-ROVER

Password:
12345678

IP:
192.168.4.1
```

Open:

```text
http://192.168.4.1
```

---

# 📊 Web Dashboard Architecture

The embedded server exposes a JSON telemetry endpoint:

```text
GET /data
```

Example drone response:

```json
{
  "latitude": 28.6139,
  "longitude": 77.2090,
  "altitude": 125.4,
  "satellites": 12,
  "lidar": 842,
  "temperature": 31.2,
  "humidity": 54.6,
  "ax": 0.21,
  "ay": -0.14,
  "az": 9.71
}
```

Example rover response:

```json
{
  "gas": 823,
  "temperature": 30.4,
  "humidity": 61.2,
  "vibration": false,
  "ax": 0.12,
  "ay": 0.08,
  "az": 9.75
}
```

The dashboard periodically requests `/data` and updates the sensor values without requiring a page refresh.

---

# 🔄 System Working

## Phase 1 — Deployment

The drone is launched and flown to the target inspection area.

```text
Operator
   ↓
Drone
   ↓
GPS positioning
   ↓
Aerial inspection
```

---

## Phase 2 — Area Detection

The drone uses:

* GPS
* LiDAR
* Camera
* IMU
* Environmental sensors

to gather information about the area.

---

## Phase 3 — Rover Deployment

Once an area requiring ground inspection is identified, the rover is deployed.

```text
Drone
  │
  ▼
Target Area
  │
  ▼
Rover Deployment
```

---

## Phase 4 — Ground Inspection

The rover moves through the target area while monitoring:

```text
Gas
 │
 ├── Environmental conditions
 │
 ├── Vibration
 │
 ├── Motion/orientation
 │
 └── Camera
```

---

## Phase 5 — Telemetry

Sensor data is transmitted wirelessly to the monitoring interface.

```text
Sensors
   ↓
XIAO ESP32S3
   ↓
Wi-Fi
   ↓
Web Server
   ↓
Dashboard
   ↓
Operator
```

---

# 🚨 Emergency Detection

The system can generate alerts when abnormal conditions are detected.

### Gas alert

```text
Gas Sensor
     ↓
ADC Reading
     ↓
Threshold Comparison
     ↓
 ┌───────────────┐
 │ Normal        │
 │ or            │
 │ HIGH GAS      │
 └───────────────┘
```

### Vibration alert

```text
SW-420 / MPU6050
       ↓
Vibration Detection
       ↓
Threshold
       ↓
⚠ VIBRATION ALERT
```

### Obstacle detection

```text
LiDAR
  ↓
Distance Measurement
  ↓
Safety Threshold
  ↓
⚠ OBSTACLE DETECTED
```

---

# 🧠 Future AI Integration

AeroRover is designed to support edge AI and computer-vision capabilities.

Potential models include:

### Drone

* Object detection
* Person detection
* Vehicle detection
* Infrastructure inspection
* Fire/smoke detection

### Rover

* Gas/environmental anomaly detection
* Damage classification
* Blockage detection
* Object detection
* Terrain classification
* Hazard detection

A future implementation can use the XIAO camera or an external Raspberry Pi/AI processor for heavier models.

---

# 📷 Camera Architecture

The XIAO ESP32S3 Sense can be used as a camera/edge device.

```text
Camera
   ↓
XIAO ESP32S3 Sense
   ↓
Image Processing
   ↓
Wi-Fi
   ↓
Operator Dashboard
```

For computationally heavier AI models, the architecture can be expanded:

```text
Camera
   ↓
XIAO / Camera Node
   ↓
Raspberry Pi / Edge Computer
   ↓
AI Model
   ↓
Detection Result
   ↓
Dashboard
```

---

# 🧪 Testing

AeroRover should be tested incrementally.

### Sensor testing

```text
☐ GPS lock
☐ LiDAR distance
☐ MPU6050 acceleration
☐ MPU6050 gyroscope
☐ BME280 temperature
☐ BME280 humidity
☐ BME280 pressure
☐ Gas sensor
☐ DHT22
☐ Vibration sensor
```

### Communication testing

```text
☐ Drone Wi-Fi
☐ Rover Wi-Fi
☐ HTTP server
☐ JSON telemetry
☐ Dashboard updates
☐ Connection recovery
```

### Mechanical testing

```text
☐ Rover motor operation
☐ Rover steering
☐ Drone motor direction
☐ ESC response
☐ Propeller balance
☐ Frame stability
```

### System testing

```text
☐ Drone inspection
☐ Rover deployment
☐ Ground navigation
☐ Sensor monitoring
☐ Emergency detection
☐ Telemetry transmission
```

---

# ⚠️ Safety Considerations

AeroRover combines batteries, high-current motors, rotating propellers and autonomous electronics.

Always:

* Remove drone propellers during bench testing.
* Verify motor direction before installing propellers.
* Use appropriately rated ESCs and power wiring.
* Use a proper LiPo charger and battery protection procedures.
* Use regulated power for ESP32 and sensors.
* Never expose ESP32 GPIOs to voltages above their permitted levels.
* Protect MQ sensor analog outputs from excessive voltage.
* Test autonomous behavior in an isolated area.
* Keep the drone away from people during flight testing.

---

# 🛠️ Troubleshooting

### XIAO does not connect

Check:

```text
USB cable
Board selection
Port selection
ESP32 board package
Power supply
```

### MPU6050 not detected

Check:

```text
SDA → D5
SCL → D6
VCC → 3.3V
GND → GND
```

and verify the I²C address.

### GPS has no position

GPS modules may require an unobstructed view of the sky.

Check:

```text
GPS TX → XIAO RX
GPS RX → XIAO TX
Common GND
Correct baud rate
Antenna orientation
```

### Gas sensor readings are unstable

MQ sensors require warm-up time and their readings depend heavily on the specific sensor, module, environment and calibration.

Do not interpret the raw ADC value as a calibrated gas concentration without proper calibration.

---

# 🔮 Roadmap

* [ ] Combined drone + rover dashboard
* [ ] Live GPS map
* [ ] Drone telemetry streaming
* [ ] Rover telemetry streaming
* [ ] Remote rover control
* [ ] Drone-to-rover communication
* [ ] Automatic rover deployment
* [ ] Camera streaming
* [ ] AI object detection
* [ ] Hazard detection
* [ ] Gas anomaly classification
* [ ] Autonomous navigation
* [ ] SLAM / mapping
* [ ] LoRa long-range telemetry
* [ ] Cloud telemetry
* [ ] Mission recording
* [ ] Automatic emergency alerts
* [ ] Battery monitoring
* [ ] Return-to-home integration
* [ ] Multi-rover expansion

---

# 🎯 Applications

AeroRover can be adapted for:

* Disaster response
* Industrial inspection
* Search and rescue
* Hazardous-area inspection
* Infrastructure inspection
* Remote environmental monitoring
* Fire and smoke reconnaissance
* Underground/isolated-area inspection
* Agricultural monitoring
* Emergency response operations

---

# 📈 Scalability

The architecture is intentionally modular.

```text
                    AeroRover Command
                           │
          ┌────────────────┼────────────────┐
          │                │                │
       Drone 1          Rover 1          Rover 2
          │                │                │
       Sensors          Sensors          Sensors
```

Additional robots can be added without redesigning the entire system.

The same telemetry architecture can also be extended to LoRa, Wi-Fi mesh, cellular communication or a cloud backend.

---

# 🧩 Design Philosophy

AeroRover follows a modular robotics approach:

```text
Sensing
   ↓
Processing
   ↓
Communication
   ↓
Decision Support
   ↓
Actuation
   ↓
Monitoring
```

The drone provides **rapid aerial reconnaissance**, while the rover provides **close-range ground inspection**.

This combination allows the system to cover a larger area while reducing the need for humans to enter potentially hazardous environments.

---

# 👨‍💻 Development

Built using:

```text
C/C++
Arduino
ESP32
XIAO ESP32S3 Sense
Embedded Systems
IoT
Robotics
Computer Vision
Web Technologies
```
---

# ⭐ Project

**AeroRover — Drone-Deployed Multi-Robot System for Remote Inspection and Emergency Response**

> **Sense from the sky. Inspect from the ground. Respond intelligently.**
