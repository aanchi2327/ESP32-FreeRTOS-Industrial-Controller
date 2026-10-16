
# ESP32 FreeRTOS Industrial Controller

## Overview
An ESP32-based industrial monitoring prototype using FreeRTOS for multitasking, sensor data transfer, and fault detection.

## Features
- FreeRTOS sensor and monitoring tasks
- Queue-based data communication
- Mutex for shared Serial output
- ADC sensor monitoring using GPIO 4
- Threshold-based fault detection
- Fault and recovery event logging
- Wokwi simulation

## Hardware and Simulation
- ESP32 DevKitC V4
- Potentiometer connected to GPIO 4
- Wokwi ESP32 simulator

## Fault Detection
- ADC value greater than 3000: Fault detected
- ADC value 3000 or below: Normal condition or recovery

## How to Test
1. Run the simulation in Wokwi.
2. Open the Serial Monitor.
3. Adjust the potentiometer.
4. Observe fault and recovery messages.

## Limitations
This is an educational simulation. It does not include certified industrial safety controls or permanent event storage.

## Future Improvements
- Watchdog integration and testing
- Persistent event logging
- MQTT communication
- Hardware validation
