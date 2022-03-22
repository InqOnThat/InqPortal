#include <InqPortal.h>

// Works with InqPortal v5
// Can use NULL if you want to configure via InqPortal Admin 
// or hard code your router SSID and Password.
#define YOUR_SSID           NULL
#define YOUR_PW             NULL
#define DEFALUT_SSID        "InqSR"
#define VERSION             "3.0.0"
#define LED                 D4
#define PHOTO1              D1
#define PHOTO2              D2
#define LOG_LEVEL           1
#define CHK_FOR_RUN         50      // Check for a valid run (ms)
#define CHK_FOR_FAULT       1000    // Check to see if there is a fault (ms)

InqPortal svr;

// Published variables
float speed = 0;
u32 finish = 0;

// Working Variables (not published)
u32 tPhoto1Blocked, tPhoto1Unblocked, tPhoto2Blocked, tPhoto2Unblocked;
bool blockage = false; // To detect a continuous blockage and flash LED slowly.

void setup() 
{
    // Set up Interval callbacks
    svr.onInterval(checkForRun, CHK_FOR_RUN);
    svr.onInterval(checkForFault, PAUSE);

    // Publish runtime and persisted variables
    svr.publishRO("V", VERSION, sizeof(VERSION), "InqSpeedRacer Version");
    svr.publishRO("S", &speed, "Speed (m/s)");
    svr.publishRO("F", &finish, "Finish (ms)");
    svr.publishRO("T", NULL, "Server time (ms)", 
        []()->u32 { return millis(); });
    svr.autoSend(0);
    
    // Start the InqPortal server.
    svr.begin(DEFALUT_SSID, NULL, YOUR_SSID, YOUR_PW); 

    pinMode(LED, OUTPUT);
    pinMode(PHOTO1, INPUT_PULLUP);
    pinMode(PHOTO2, INPUT_PULLUP);  
    clear();
}

#define isBlocked(p) digitalRead(p)
#define LED_TOGGLE digitalWrite(LED, !digitalRead(LED))
#define LED_OFF digitalWrite(LED, true)

void loop() 
{
    // Sensor 1
    if (!tPhoto1Blocked)
    {
        if (isBlocked(PHOTO1))
        {
            tPhoto1Blocked = micros();
            set();
        }
    }
    else if (!tPhoto1Unblocked)
    {
        if (!isBlocked(PHOTO1))
            tPhoto1Unblocked = micros();
    }
    // Sensor 2
    if (!tPhoto2Blocked)
    {
        if (isBlocked(PHOTO2))
        {
            tPhoto2Blocked = micros();
            set();
        }
    }
    else if (!tPhoto2Unblocked)
    {
        if (!isBlocked(PHOTO2))
            tPhoto2Unblocked = micros();
    }
}

void set()
{
    svr.onInterval(checkForFault, CHK_FOR_FAULT);
    blockage = true;
    if (!finish)
        finish = millis();
    LED_OFF;
}

void clear()
{
    tPhoto1Blocked  = 0;
    tPhoto1Unblocked = 0;
    tPhoto2Blocked  = 0;
    tPhoto2Unblocked = 0;
    finish = 0;
    svr.onInterval(checkForFault, PAUSE);
    blockage = false;
}

void checkForRun(void*)
{
    if (!blockage)
        LED_TOGGLE;
        
    if (tPhoto1Blocked && tPhoto1Unblocked && 
        tPhoto2Blocked && tPhoto2Unblocked)
    {
        // WARNING - Don't mess with this.  Needed to handle wrapping
        // of the u32 coming from the micros()!!
        s32 t = (abs((s32)(tPhoto1Blocked - tPhoto2Blocked)) + 
                 abs((s32)(tPhoto2Unblocked - tPhoto1Unblocked))) / 2;
        // This does the speed calculation based on the distance
        // between the two sensors of 19.7 mm and time in micro seconds.
        speed = 19700.0 / (float)t;   // m/s
        // Sends it out the the clients.
        svr.send("lflu", "S", speed, "F", finish);
        clear();
    }
}

void checkForFault(void*)
{
    // We added this in case we have false trigger.  After duration  
    // of CHK_FOR_FAULT (ms), it will clear out the times.
    // This checks if its still blocked... then we know something
    // is in the way.  We blink slowly.
    blockage = isBlocked(PHOTO1) | isBlocked(PHOTO2);     // Still blocked?

    if (blockage)
        LED_TOGGLE;
    else
    {
        svr.LOG(LOG_LEVEL, "Fault detected and cleared\n");
        clear();  
    }
}
