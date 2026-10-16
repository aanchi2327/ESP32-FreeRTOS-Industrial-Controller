
#include <Arduino.h>

// ---------- Configuration ----------
#define SENSOR_PIN 4
#define ADC_THRESHOLD 3000

// ---------- Sensor Data Structure ----------
struct SensorData {
  int adcValue;
  uint32_t timestamp;
};

// ---------- FreeRTOS Objects ----------
QueueHandle_t sensorQueue;
SemaphoreHandle_t serialMutex;

// ---------- Fault State ----------
bool faultActive = false;

// ---------- Sensor Task ----------
void sensorTask(void *parameter) {
  SensorData data;

  while (true) {
    data.adcValue = analogRead(SENSOR_PIN);
    data.timestamp = millis();

    // Send the latest sensor reading to the monitor task
    xQueueOverwrite(sensorQueue, &data);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// ---------- Monitor and Fault Recovery Task ----------
void monitorTask(void *parameter) {
  SensorData data;

  while (true) {
    if (xQueueReceive(sensorQueue, &data, portMAX_DELAY) == pdTRUE) {
      bool currentFault = data.adcValue > ADC_THRESHOLD;

      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {

        if (currentFault && !faultActive) {
          Serial.println("\n========== FAULT EVENT ==========");
          Serial.printf("Time: %lu ms\n",
                        (unsigned long)data.timestamp);
          Serial.printf("ADC Value: %d\n", data.adcValue);
          Serial.println("Status: FAULT DETECTED");
          Serial.println("=================================\n");
        }
        else if (!currentFault && faultActive) {
          Serial.println("\n========= RECOVERY EVENT ========");
          Serial.printf("Time: %lu ms\n",
                        (unsigned long)data.timestamp);
          Serial.printf("ADC Value: %d\n", data.adcValue);
          Serial.println("Status: SYSTEM RECOVERED");
          Serial.println("=================================\n");
        }
        else if (!currentFault) {
          Serial.printf("[NORMAL] ADC: %d | Time: %lu ms\n",
                        data.adcValue,
                        (unsigned long)data.timestamp);
        }

        xSemaphoreGive(serialMutex);
      }

      faultActive = currentFault;
    }
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(SENSOR_PIN, INPUT);
  delay(1000);

  sensorQueue = xQueueCreate(1, sizeof(SensorData));
  serialMutex = xSemaphoreCreateMutex();

  if (sensorQueue == nullptr || serialMutex == nullptr) {
    Serial.println("ERROR: FreeRTOS initialization failed!");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("======================================");
  Serial.println("ESP32 FreeRTOS Industrial Controller");
  Serial.println("Sensor Monitoring: Enabled");
  Serial.println("Fault Detection: Enabled");
  Serial.println("Fault Recovery Logging: Enabled");
  Serial.printf("ADC Threshold: %d\n", ADC_THRESHOLD);
  Serial.println("======================================");

  if (xTaskCreate(sensorTask, "SensorTask", 4096,
                  nullptr, 2, nullptr) != pdPASS) {
    Serial.println("ERROR: Sensor task creation failed!");
    while (true) {
      delay(1000);
    }
  }

  if (xTaskCreate(monitorTask, "MonitorTask", 4096,
                  nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Monitor task creation failed!");
    while (true) {
      delay(1000);
    }
  }
}

// ---------- Main Loop ----------
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
