// FirstPortal.ino Complete

// Include the InqPortal library
// Usage - https://inqonthat.com/inqportal-bare-essentials/
#include <InqPortal.h>

// Uses the analog pin A0 to get supply voltage.
// This is sent out with the other performace metrics.
// Usage - https://inqonthat.com/inqportal-adding-performance-metrics/#ADC_MODE
ADC_MODE(ADC_VCC);

// This is a custom C++ struct for your persisted variables.  
// For this example, we use it to adjust results of our temperature sensor
// for "calibration" purposes.  InqPortal recognizes all numeric types,
// including s8, u8, s16, u16, s32, u32, float, double and of course all the
// long winded versions if you prefer.  It also supports fixed-length 
// character buffers.  It does NOT support Arduino String objects!
// Use of persisted variables is optional.  If you don't need any persisted
// data, you can skip this part.
struct FirstPersist
{
    float m;
    float b;
    char room[32];
};

// If you need persisted data, now make an instance of your object and 
// initialize it with your default values.  If you interact with the UI and
// and change these values, they will be automatically persisted in case of
// power failure.  The persisted values will override these defaults on 
// power up.
FirstPersist config = { 1.0, 0.0, "Garage" };

// Declare an InqPortal server, with the address of your custom persist
// struct and its size.  If you don't need persisted data, the default
// constructor ( InqPortal svr; )  is sufficient.
// Usage - https://inqonthat.com/inqportal-bare-essentials/
// Reference - https://inqonthat.com/inqportal-class-reference/#InqPortal
InqPortal svr((u8*)&config, sizeof(FirstPersist));

// Define any of your published varibles before setup() so they can be 
// accessed by your Sketch code as well as InqPortal.  Being outside of
// the your custom persist struct means when the power goes out, they will
// be lost.  In this example, this is a runtime variable for temperature.  
float temperature = 0;

void setup() 
{
  // Add a heading on our Admin "App" tab.  Purely optional and cosmetic.
  // Usage - https://inqonthat.com/inqportal-pushing-data/#variable
  // Reference - https://inqonthat.com/inqportal-class-reference/#heading
  svr.heading("LW", "Run-Time Variables");
  
  // Add a published run-time variable that you want your clients to have
  // access.  RO - stands for readonly.  It is read only by your clients.
  // Of course the Sketch can change it at any time.
  // Temp - is the published variable name and how it will be referenced in
  // the clients.  It must start with a capital letter.
  // &temperature - Note:  You are giving the address to your Sketch variable. 
  // "Temperature..." - This is the friendly label used in the Admin.
  // More details and other options for defining your published variables can
  // be found:
  // Usage - https://inqonthat.com/inqportal-pushing-data/#variable
  // Reference - https://inqonthat.com/inqportal-class-reference/#variable
  svr.publishRO("Temp", &temperature, "Temperature (°C)"); 

  // Add a second heading on our Admin "App" tab.
  svr.heading("LP", "Persisted Editable Variables");

  // Define our published variables to access our persisted values.  
  // RW - stands for read/write.  This allows the client to make changes
  // to these values.  The method parameters are the same as publishRO().
  svr.publishRW("Room", config.room, sizeof(config.room), 
    "Temperature in room");
  svr.publishRW("M", &config.m, "Slope"); 
  svr.publishRW("B", &config.b, "Offset"); 

  // Set up an Interval callback.  Similar to Ticker, but integrated into
  // InqPortal.  It is NOT based on Interrupts, so you do not have to be
  // concerned about interrupt synchronization issues.
  // Usage - https://inqonthat.com/inqportal-pushing-data/#intervalCallbacks
  // Reference - https://inqonthat.com/inqportal-class-reference/#onInterval
  svr.onInterval(sendTemperature, 1000);

  // Start the InqPortal server.  
  // All the parameters for begin() are optional.  During development, or if
  // you're building an app just for yourself, you might want to define the
  // connections.  Once you distribute, or sell your project, you can remove
  // the parameters and use the Admin to configure them like IoT products do.
  // (1) SSID for the SoftAP (Built-in access point)
  // (2) Password for the SoftAP.
  // (3) SSID for the router you want it to connect.
  // (4) Password for the router you want it to connect.
  // For more information and options, see:
  // Usage - https://inqonthat.com/inqportal-bare-essentials/
  // Reference - https://inqonthat.com/inqportal-class-reference/#begin
  svr.begin("FirstPortal", NULL, 
    "<your router SSID>", "<your router Password>");
}

void loop() 
{
    // Using multiple onInterval() to define routine callbacks frees up the 
    // loop() method for tasks that need high rates ~100 kHz.  This 
    // permits a cleaner, best practice of segregating concerns.
}

void sendTemperature(void* tag)
{
    // Our callback every 1 second.  Call our routine that deals with the
    // "sensor" and set our published variable.
    temperature = getTemperature();
}

float getTemperature()
{
    // We'll use a nice smooth trigonometry function so we can the results
    // can be qualitatively evaluated.
    #define MIN_P_CYCLE 1
    #define MS_P_MIN 60000
    u32 fraction = millis() % (MS_P_MIN * MIN_P_CYCLE);
    float alpha = (float)(fraction / (float)(MS_P_MIN * MIN_P_CYCLE)) *
        2.0 * PI;
    return (float)(sin(alpha) * 100.0);
}