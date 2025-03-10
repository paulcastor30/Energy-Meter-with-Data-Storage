#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

// put your instances here:
Adafruit_INA219 ina219;
RTC_DS3231 rtc;

// put variable declarations here:
char buffer[50];
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// put function declarations here:

struct A
{
    /* data */
    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;
};

struct A get_data()
{
    struct A to_return;

    to_return.shuntvoltage = ina219.getShuntVoltage_mV();
    to_return.busvoltage = ina219.getBusVoltage_V();
    to_return.current_mA = ina219.getCurrent_mA();
    to_return.power_mW = ina219.getPower_mW();
    to_return.loadvoltage = to_return.busvoltage + (to_return.shuntvoltage / 1000);

    return to_return;
}

void setup()
{
    // put your setup code here, to run once:
    // int result = myFunction(2, 3);

    Serial.begin(115200);
    while (!Serial)
    {
        delay(1);
    }

    Serial.println("Program Initializing!");

    if (!ina219.begin())
    {
        Serial.println("Failed to find INA219 chip");
        while (1)
        {
            delay(10);
        }
    }

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    Serial.println("Initialization done...");
}

void loop()
{
    // put your main code here, to run repeatedly:
    struct A res = get_data();
    sprintf(buffer, "shuntvoltage =  %.2f, busvoltage = %.2f, current_mA =  %.2f, power_mW = %.2f, loadvoltage =  %.2f", res.shuntvoltage, res.busvoltage, res.busvoltage, res.power_mW, res.loadvoltage);
    Serial.println(buffer);
}

// put function definitions here:
int myFunction(int x, int y)
{
    return x + y;
}