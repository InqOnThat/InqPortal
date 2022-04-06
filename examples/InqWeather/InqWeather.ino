#include <InqPortal.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>
#include "Zambretti.h"

// ============================= WIRING ========================================
// See https://inqonthat.com/inqweather/ for more details.
// This is representative for NodeMCU and WeMos ESP8266 development boards
// Place the AHT10 and BMP180 break-out boards on your bread board such that
// the VIN, GND, SCL and SDA pins share the same connections
// Needs 4 wires total.
// Red    - NodeMCU "3V" to AHT10/BMP180 common VIN
// Black  - NodeMCU "G"  to AHT10/BMP180 common GND
// Yellow - NodeMCU "D1" to AHT10/BMP180 common SCL
// Orange - NodeMCU "D2" to AHT10/BMP180 common SDA
// =============================================================================

ADC_MODE(ADC_VCC);

// Can use NULL if you want to configure via InqPortal Admin
#define YOUR_SSID NULL
#define YOUR_PW NULL
#define DEFAULT_HOST_SSID "InqWeather"
#define VERSION_INQ_WEATHER "5.0.0"          
#define LOG_LEVEL 1

struct InqWeatherPersisted
{
    float altitude;   // Factor to adjust pressure for altitude.
    float zMinP;      // Zambretti Variable
    float zMaxP;      // Zambretti Variable
    u8 zNorth;        // Northern hemisphere = 1, Southern hemisphere = 0
};

InqWeatherPersisted config = { 1.0, 950, 1050, 1 };
InqPortal svr((u8*)&config, sizeof(InqWeatherPersisted));

Adafruit_BMP085 bmp180;     // Uses standard I2C bus - SCL=D1, SDA=D2
Adafruit_AHTX0 aht10;       // Uses standard I2C bus - SCL=D1, SDA=D2

#define READING_INTERVAL 1000   
#define READINGS_PER_MINUTE 60000 / READING_INTERVAL

// The "current" published variables (xxxxxCur) are really not needed for
// the functionality of the Weather Station, but we wanted to see a little
// more action in the GUI than 1 minute updates!

// Published Pressure Variables
float temperatureBMP180Cur = 0;     // °C
float pressureBMP180Cur = 0;        // mbar
float temperatureBMP180Min = 0;     // °C
float pressureBMP180Min = 0;        // mbar
float pressureDelta3Hour = 0;       // mbar
// Published Humidity Variables
float temperatureAHT10Cur = 0;      // °C
float humidityAHT10Cur = 0;         // %H
float temperatureAHT10Min = 0;      // °C
float humidityAHT10Min = 0;         // %H

// Zambretti Prediction
u8 zam = 0;                   // 0 - 25 (A-Z of Zambretti Scale)

// =============================================================================

void setup() 
{
    svr.onInterval(takeReading, READING_INTERVAL);  

    svr.publishRO("V", VERSION_INQ_WEATHER, sizeof(VERSION_INQ_WEATHER),
        "InqWeather Version");
    
    svr.heading("LS", "Settings");
    svr.publishRW("North", &config.zNorth, "Northern Hemisphere (bool)");
    svr.publishRW("Alt", &config.altitude, "Altitude adjustment", NULL,
        validateAltitudeFactor);
    svr.publishRW("Pmin", &config.zMinP, "Area Min Pressure (mbar)");
    svr.publishRW("Pmax", &config.zMaxP, "Area Max Pressure (mbar)");
    
    svr.heading("LH", "AHT10 Humidity Sensor");
    svr.publishRO("Th1", &temperatureAHT10Cur, "Current Temperature (°C)"); 
    svr.publishRO("H1", &humidityAHT10Cur, "Current Humidity (%H)");
    
    svr.heading("LP", "BMP180 Pressure Sensor");
    svr.publishRO("T1", &temperatureBMP180Cur, "Current Temperature (°C)");
    svr.publishRO("P1", &pressureBMP180Cur, "Current Pressure (mbar)");
    
    svr.heading("LA", "Minute Averages");
    svr.publishRO("Th", &temperatureAHT10Min, "Temperature (°C)"); 
    svr.publishRO("H", &humidityAHT10Min, "Humidity (%M)");
    svr.publishRO("Tp", &temperatureBMP180Min, "Temperature (°C)");
    svr.publishRO("P", &pressureBMP180Min, "Pressure (mbar)");
    
    svr.heading("LZ", "Calculations");
    svr.publishRO("D3P", &pressureDelta3Hour, "3 Hour Pressure Change (mbar)");
    svr.publishRO("Z", &zam, "Zambretti (0-25) = (A-Z)");

    svr.begin(DEFAULT_HOST_SSID, NULL, YOUR_SSID, YOUR_PW);

    pinMode(LED_BUILTIN, OUTPUT);
    
    if (bmp180.begin())
        Serial.printf("BMP180 init success\n");
    else
        Serial.printf("BMP180 init fail\n");
    
    if (aht10.begin())
        Serial.printf("AHT10 init success\n");
    else
        Serial.printf("AHT10 init fail\n");
}

// =============================================================================

void loop(){}    

// =============================================================================

void validateAltitudeFactor(float factor)
{
    // Minimum and Maximum altitude variations on earth...
    factor = min(3.0F, max(0.95F, factor));
    
    // Need to adjust zMaxP and zMinP if they've been accumulating with
    // the old factor.
    config.zMaxP *= factor / config.altitude;
    config.zMinP *= factor / config.altitude;
    // Update the factor.
    config.altitude = factor;
}

// =============================================================================

void takeReading(void* tag)
{    
    // Blinky the LED just so we know its alive!
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    // AHT10 Temperature / Humidity Sensor -------------------------------------
    static u8 C10 = 0;
    static float T10 = 0;  
    static float H10 = 0;

    sensors_event_t humid, temp;
    aht10.getEvent(&humid, &temp);

    temperatureAHT10Cur = temp.temperature;
    humidityAHT10Cur = humid.relative_humidity;
    T10 += temperatureAHT10Cur;
    H10 += humidityAHT10Cur;
    C10++;
    if (C10 == READINGS_PER_MINUTE)
    {
        temperatureAHT10Min = T10 / (float)C10;
        humidityAHT10Min = H10 / (float)C10;
        T10 = H10 = C10 = 0;
    }

    // BMP180 Temperature / Pressure Sensor ------------------------------------
    static u8 C180 = 0;
    static float T180 = 0;  
    static float P180 = 0;

    temperatureBMP180Cur = bmp180.readTemperature();
    pressureBMP180Cur = (float)bmp180.readPressure() * 
        0.01 *              // Convert to mbar
        config.altitude;    // Convert to mbar at sea-level

    if (pressureBMP180Cur > 850)    // World record low pressure
    {
        // Needed for Zambretti Algorhithm 
        if (pressureBMP180Cur > config.zMaxP)
            config.zMaxP = pressureBMP180Cur;
        if (pressureBMP180Cur < config.zMinP)
            config.zMinP = pressureBMP180Cur;    

        T180 += temperatureBMP180Cur;
        P180 += pressureBMP180Cur;  
        C180++;
        if (C180 == READINGS_PER_MINUTE)
        {
            temperatureBMP180Min = T180 / (float)C180;
            pressureBMP180Min = P180 / (float)C180;
            T180 = P180 = C180 = 0;
            
            // Calculate the 3 hour pressure change for weather prediction.
            pressureDelta3Hour = doPressureChange(pressureBMP180Min);    
            zam = zambretti(pressureBMP180Min, pressureDelta3Hour, 
                config.zMinP, config.zMaxP, config.zNorth);
                
            static u8 oldzam = 100;
            if (zam != oldzam)
            {
                svr.LOG(LOG_LEVEL, "Zambretti change %u to %u\n",
                    oldzam, zam);
                oldzam = zam;
            }
        }
    }
    else
        svr.LOG(LOG_LEVEL, "Bad pressure reading = %f mbar\n", 
            pressureBMP180Cur);
}

// =============================================================================

