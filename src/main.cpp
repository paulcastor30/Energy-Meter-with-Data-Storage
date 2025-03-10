/*
 *Project: Power/Energy Detection with Data Storage
 *Developer: Paul Rodolf P. Castor
 *Date: February 2025
 *Version: 1.5
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Adafruit_INA219.h>

// Task Handles
TaskHandle_t TaskReadRTC_Handle;
TaskHandle_t TaskReadINA219_Handle;
TaskHandle_t TaskLogToSD_Handle;

// Global Objects
RTC_DS3231 rtc;
Adafruit_INA219 ina219;
File dataFile;

// SD Card Pin
const int chipSelect = 5;

// Shared Global Variables (Protected by FreeRTOS)
struct SensorData
{
  DateTime now;
  float shuntVoltage;
  float busVoltage;
  float current_mA;
  float power_mW;
} sharedData;

// Mutex for Data Protection
SemaphoreHandle_t dataMutex;

// Function to Retry Initialization
bool initializeRTC()
{
  int retry = 3;
  while (retry--)
  {
    if (rtc.begin())
    {
      if (rtc.lostPower())
      {
        Serial.println("RTC lost power, setting time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
      return true;
    }
    Serial.println("Failed to find RTC. Retrying...");
    delay(1000);
  }
  Serial.println("RTC Initialization Failed!");
  return false;
}

bool initializeINA219()
{
  int retry = 3;
  while (retry--)
  {
    if (ina219.begin())
    {
      return true;
    }
    Serial.println("Failed to find INA219 sensor. Retrying...");
    delay(1000);
  }
  Serial.println("INA219 Initialization Failed!");
  return false;
}

bool initializeSDCard()
{
  int retry = 3;
  while (retry--)
  {
    if (SD.begin(chipSelect))
    {
      Serial.println("SD Card initialized successfully!");
      return true;
    }
    Serial.println("SD Card initialization failed. Retrying...");
    delay(1000);
  }
  Serial.println("SD Card Initialization Failed!");
  return false;
}

// Task 1: Read RTC Time
void TaskReadRTC(void *pvParameters)
{
  for (;;)
  {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY))
    {
      sharedData.now = rtc.now();
      xSemaphoreGive(dataMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
  }
}

// Task 2: Read INA219 Sensor
void TaskReadINA219(void *pvParameters)
{
  for (;;)
  {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY))
    {
      sharedData.shuntVoltage = ina219.getShuntVoltage_mV();
      sharedData.busVoltage = ina219.getBusVoltage_V();
      sharedData.current_mA = ina219.getCurrent_mA();
      sharedData.power_mW = ina219.getPower_mW();
      xSemaphoreGive(dataMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
  }
}

// Task 3: Log Data to SD Card
void TaskLogToSD(void *pvParameters)
{
  for (;;)
  {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY))
    {
      char buffer[100];

      // Format data as a CSV line
      sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d, %.2f, %.2f, %.2f, %.2f",
              sharedData.now.year(), sharedData.now.month(), sharedData.now.day(),
              sharedData.now.hour(), sharedData.now.minute(), sharedData.now.second(),
              sharedData.shuntVoltage, sharedData.busVoltage,
              sharedData.current_mA, sharedData.power_mW);

      Serial.println(buffer); // Print to Serial Monitor

      // Write to SD card with error handling
      dataFile = SD.open("/log.txt", FILE_APPEND);
      if (dataFile)
      {
        dataFile.println(buffer);
        dataFile.close();
      }
      else
      {
        Serial.println("⚠️ Error opening log file! Retrying in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait before retrying
        continue;                        // Skip this iteration and try again
      }

      xSemaphoreGive(dataMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(2000)); // Log every 2 seconds
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C

  // Initialize RTC
  if (!initializeRTC())
  {
    Serial.println("RTC failed, entering safe mode.");
  }

  // Initialize INA219
  if (!initializeINA219())
  {
    Serial.println("INA219 failed, entering safe mode.");
  }

  // Initialize SD Card
  if (!initializeSDCard())
  {
    Serial.println("SD Card failed, logging disabled.");
  }

  // Initialize Mutex
  dataMutex = xSemaphoreCreateMutex();
  if (dataMutex == NULL)
  {
    Serial.println("Error creating Mutex!");
    while (1)
      ;
  }

  // Create FreeRTOS Tasks
  xTaskCreatePinnedToCore(TaskReadRTC, "Read RTC", 2048, NULL, 1, &TaskReadRTC_Handle, 1);
  xTaskCreatePinnedToCore(TaskReadINA219, "Read INA219", 2048, NULL, 1, &TaskReadINA219_Handle, 1);
  xTaskCreatePinnedToCore(TaskLogToSD, "Log to SD", 4096, NULL, 1, &TaskLogToSD_Handle, 1);
}

void loop()
{
  // Empty, FreeRTOS handles execution
}
