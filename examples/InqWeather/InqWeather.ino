#include <InqPortal.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>

// ============================= WIRING ========================================
// See https://inqonthat.com/inqweather/ for more details.
// This is representative for NodeMCU and WeMos ESP8266 development boards
// Place the AH10 and BMP180 break-out boards on your bread board such that
// the VIN, GND, SCL and SDA pins share the same connections
// Needs 4 wires total.
// Red    - NodeMCU "3V" to AH10/BMP180 common VIN
// Black  - NodeMCU "G"  to AH10/BMP180 common GND
// Yellow - NodeMCU "D1" to AH10/BMP180 common SCL
// Blue   - NodeMCU "D2" to AH10/BMP180 common SDA

// =============================================================================

ADC_MODE(ADC_VCC);

#define DEFAULT_HOST_SSID "InqWeather"
#define VERSION_INQ_WEATHER "4.0.0"          
#define LOG_LEVEL 1

// Can use NULL if you want to configure via InqPortal Admin
#define YOUR_SSID NULL      // "Your routers SSID"   
#define YOUR_PW NULL        // "Your routers Password"

struct InqWeatherPersisted
{
  float altitude;   // Factor to adjust pressure for altitude.
};

// https://barometricpressure.app/results?lat=35.4125&lng=-83.2992
InqWeatherPersisted config = { 1.0795 };
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
float pressureBMP180Cur = 0;        // in-Hg
float temperatureBMP180Min = 0;     // °C
float pressureBMP180Min = 0;        // in-Hg
float pressureDelta3Hour = 0;       // in-Hg
// Published Humidity Variables
float temperatureAHT10Cur = 0;      // °C
float humidityAHT10Cur = 0;         // %H
float temperatureAHT10Min = 0;      // °C
float humidityAHT10Min = 0;         // %H

// =============================================================================

void setup() 
{
    svr.onInterval(takeReading, READING_INTERVAL);  

    svr.publishRO("V", VERSION_INQ_WEATHER, sizeof(VERSION_INQ_WEATHER),
        "InqWeather Version");
    
    svr.heading("LP", "BMP180 Pressure Sensor");
    svr.publishRO("T1", &temperatureBMP180Cur, "Current Temperature (°C)");
    svr.publishRO("P1", &pressureBMP180Cur, "Current Pressure (in-Hg)");
    svr.publishRO("Tp", &temperatureBMP180Min, "Average Temperature (°C)");
    svr.publishRO("P", &pressureBMP180Min, "Average Pressure (in-Hg)");
    svr.publishRO("D3P", &pressureDelta3Hour, "3 Hour Pressure Change (in-Hg)");

    svr.heading("LH", "AHT10 Humidity Sensor");
    svr.publishRO("Th1", &temperatureAHT10Cur, "Current Temperature (°C)"); 
    svr.publishRO("H1", &humidityAHT10Cur, "Current Humidity (%H)");
    svr.publishRO("Th", &temperatureAHT10Min, "Avarge Temperature (°C)"); 
    svr.publishRO("H", &humidityAHT10Min, "Average Humidity (%M)");

    svr.heading("L", "Persisted");
    svr.publishRW("Alt", &config.altitude, "Altitude adjustment");

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

void takeReading(void* tag)
{    
    // This InqPortal callback gets called every READING_INTERVAL milliseconds.
    // Static local variables to accumulate values for the averaging.
    static u8 Count = 0;
    static float T10 = 0;   // AHT10
    static float H10 = 0;
    static float T180 = 0;  // BMP180
    static float P180 = 0;

    // Set "current" published variables and accumulate minute average values.
    
    // Get AH10 Data
    sensors_event_t humid, temp;
    aht10.getEvent(&humid, &temp);
    temperatureAHT10Cur = temp.temperature;
    humidityAHT10Cur = humid.relative_humidity;
    T10 += temperatureAHT10Cur;
    H10 += humidityAHT10Cur;

    // Get BMP180 Data
    temperatureBMP180Cur = bmp180.readTemperature();
    pressureBMP180Cur = (float)bmp180.readPressure() * 
        0.0002952998 *                          // hPa Convert to in-Hg
        config.altitude;                        // Convert to sea-level
    T180 += temperatureBMP180Cur;
    P180 += pressureBMP180Cur;

    Count++;
    // Blinky the LED just so we know its alive!
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    // See if we have accumulated a minutes worth of data.
    if (Count < READINGS_PER_MINUTE)
        return;
    
    // We'll take the average of the readings over the full minute.
    // and set published variables.
    temperatureAHT10Min = T10 / Count;
    humidityAHT10Min = H10 / Count;
    

    temperatureBMP180Min = T180 / Count;
    pressureBMP180Min = P180 / Count;
    Count = T10 = H10 = T180 = P180 = 0;
    
    // Calculate the 3 hour pressure change for weather prediction.
    doPressureChange(pressureBMP180Min);    
}

// =============================================================================

void doPressureChange(float p)
{
    // Create a min-by-min circular buffer to store 3 hours of pressure
    // readings.
    #define STORE 3             // Hours
    #define CNT 60 * STORE      // One reading for each minute for 3 hours.
    static u16 next = 0;        // Where to place the next reading.
    static bool filled = false; // Flag that we've filled array at least once.
    static float* press = NULL; // The array!
    
    if (!press)
    {
        press = (float*)os_malloc(CNT * sizeof(float));
        os_memset(press, 0, sizeof(CNT * sizeof(float)));
    }
    press[next] = p;
    next++;
    if (next == CNT)    // We've filled the array, start overwriting 
    {
        svr.LOG(LOG_LEVEL, "3 hours wrap\n");
        filled = true;
        next = 0;       // from the beginning again.
    }
    
    // Calculate and publish 3 hour pressure differential.
    if (filled)
        pressureDelta3Hour = (float)(p - press[next]);
    else
        pressureDelta3Hour = (p - press[0]);
}

// =============================================================================
