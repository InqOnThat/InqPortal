// Copyright (c) 20217-2022 Dennis E. Cox. All rights reserved.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef INQ_PORTAL
#define INQ_PORTAL

#ifndef ARDUINO_ARCH_ESP8266
  #error "This library only supports ESP8266 based boards."
#endif

#include <Inq.h>

// Version History
#define InqPortalVersion "5.1.0"
#define CannedVersion    "500"   
// 0.8.0    2017/02/01 Intitial Alpha version (many rewrites using various
//          methodolgies.      
// 0.9.0    2020/01/15 Initial Version - complete re-write.  Previous version
//          did not use ESPAsyncWebServer.  It used the raw, low-level Espressif
//          routines and bypased all ESP8266WiFi library calls.  This broke
//          after version 2.3 of Arduino download.   
//          * Incorporates SPIFFS file system.
//          * If no url is present, assumes index.html
//          * If route is not explicitedly defined, SPIFFS file system is 
//              checked for existence of file.
//          * If file still does not exist, checks for a custom 404.html file.
//          * If 404.html file does not exist, uses canned 404 file.
//          * Uses AsyncWebServer template substitution only on *.html files.
// 0.9.1    * Removed access to AsyncWebServer template substitution.  Instead,
//              choose to use AJAX getters and setters via serverajax.js.
//          * Made callbacks for AJAX getter and AJAX setter.
// 0.9.2	* Re-do on Example
// 0.9.3	* Re-do on Example
// 0.9.4    * Externalize debug by using callback to post message.
//              This is support for InqVent so we can turn off debugging.
// 0.9.5    * Found bug in pins.js the accumulated callbacks till it continous.
//          * Changed pins.js to serve.js since its more generalized than pins.
// 1.0.0    2020/5/11 
//          * WebSockets... Oh yeah!
// 1.0.1    2020/7/11 
//          * Macros for streamlining json sends.
//          * Removed begin/end transaction callbacks.  GetWS handles them.
// 2.0.0    2020/7/20
//          * Removed AJAX support.  Only use websockets.
//          * Got rid of most of the static methods / fields
//          * Made connecting async.
//          * Add support for Settings configuration page.
//          * Refine Canned so we can put entire website in memory.
//          * WS library support input fields
//          * WS support changing all "class" fields
//          * Canned content is GZip
//          * File uploads with file manager tab
//          * OTA compile upload / reboot
// 2.2.0    2021/3/2
//          * Fully canned Admin
//          * Logging tab, added diagnostics of all user parameters sent from
//              server.  
//          * Library users can only use command/messages starting with 
//              uppercase leters.  All others are reserved for the library
//              including inheriting classes.
//          * Full screen on Android
// 2.3.0    2021/8/10
//          * Arrange for binary library generation.
//          * Performance Metrics - Voltage, Free Memory, Loop Frequency
//          * InqClock integration, StartTime, Time parameters
//          * Browser side changes to handle time in UI
//          * HostName same as SSID - on some routers can do
//              http://hostName/admin.html
//          * Make class of canned, removed histo.js
//          * Generalized Histogram, added charting.js
// 2.4.0    2021/8/17
//          * Integrate Inq.h so only one library is published.
//          * Updated Help to go to InqOnThat.com/InqPortal/
//          * Changed some send data method names for consistency.
//          * Added javascript to send input fields as parameters to server.
//          * JSON ignores any format characters it does not recognize.
//          * On clearing log, reset _last variable in admin.html.
// 3.0.0    2021/9/10
//          * (BREAKING CHANGE) - Replace SPIFFS with LittleFS 
//          * Improved bug rebooting occasional file uploads - appears to 
//              be based on poor WiFi connections getting disconnected during
//              uploads.
//          * WebSocket can now be used to connect to multiple servers.
//          * JS side to send all input fields.
// 3.0.1    2021/9/13 - Broke sample program.
// 4.0.0    2021/9/16
//          * (BREAKING CHANGE) - "variable" infrastructure
//              - Removed onGet, onSetFloat, onSetString, onSetInteger 
//                  callback methods.
//              - No longer need to call get in onConnected, getall returns
//                  all "variables".  Want subset, use get in onConnected.
//              - Does all set logic under the covers.  No user if tree req.
//              - Does all get logic under the covers.  No user if tree / A
//              - Validation callbacks when desired for notification/updating
//              - Automatic broadcasting of changes
//              - Automatic save of persisted data.
//          * Fix - Added ws on JS onBinary(ws)
//          * App tab added to Admin to allow adding user variables.
//          * HostSSID/ComputerName displayed on Header
//          * Admin/User JSON objects so they do not trouce on each other.
//          * Auto detect value change and send out.
//          * If client SSID is not available, will attempt time sync from a 
//              client connecting on Host.
// 5.0.0    2022/3/1 - Lone Wolf
//          SERVER-SIDE CHANGES
//          * Blank sheet Re-write of WiFi, Web Server and WebSockets using
//              Espressif, native code and lwIP only.  No longer using any 
//              ESPAsyncWebServer or Arduino Core's WiFi.  Obviously, still
//              uses some basic Core functionality since it compiles using
//              Arduino IDE.  Still uses a few things like millis() and 
//              functions out of ESP.  Even though extra capability was
//              added relative to v4, the binary is smaller.  Using HTTP file
//              upload versus WebSocket upload.  Far more robust and far 
//              higher throughput for uploading files and OTA bin files.
//              File servering throughput jumped 3x.
//          * Web Server now supports Browser caching.  After file is first 
//              served, second browsing is already there... like NOW.
//          * Not using Espressif's ability to hold multiple station AP
//              configurations.  There was no "forget" capability.  
//          * Using custom client connection methodolgy.  Ability to add and
//              forget APs.  If the connection drops, will scan searching for
//              the next strongest, configured AP.  Acts more like a laptop
//              or smart phone.  
//          * (BREAKING CHANGE) InqFS - Custom file system replacing LittleFS.
//              It is NOT a generalized file system.  Optimized for file
//              storage (uploading and file servering).  Incorporates flash
//              memory wear leveling.  Has manual Defrag.  File creation /
//              changes safe from power failure.  Old file is retained and
//              usable until changed file is completely and safely closed.
//              File timestamp added.  Timestamp is NOT when it was created
//              on InqPortal, but is Date Modified time of file on original
//              device that uploaded.  This allows visual comparison of  
//              timestamp.  Appologies to the Linux, Mac users.  Don't know
//              what those devices show on their file explorers.
//          * Canned files are all sent gzipped.  Server hosted files can
//              be gzipped as well.  ie. index.html.gz, index.cs.gz, etc.
//          * (BREAKING CHANGE) InqPersist - Custom Persisted data sub-system
//              to replace EEPROM.  Has flash wear leveling.  Can have
//              multiple instances or use more flash sectors.  Other uses
//              planned for future versions.
//          * (BREAKING CHANGE) InqPublish - All publishing of variables 
//              support incorporated into a class.  Exposed methods stream-
//              lined.  Add support for more advanced control like using as
//              events (button press), server side validation (set) and 
//              client requested running of code (get).
//          * If no Admin clients are running, server is at full tilt serving
//              only user's Sketch's needs.  No unrequested background usage.
//          * Server timestamp is sent on all messages.  v4 Histogram used 
//              client receive time of messages as the X plotted value.  This
//              caused erroneous x/y graphing, especially noticable on smooth
//              test functions like sin().  v5 using server time at data 
//              generated time shows perfectly smooth sin() curves.
//          * Time (based on millis()) can successfully wrap and show server
//              up-times greater than 49.7 days.
//          * Total data process can also wrap - Showing data greater than
//              2^32.
//          * Auto persist using autoSend() cycle.
//          CLIENT-SIDE CHANGES
//          * More robust and faster client re-connect.
//          * Simplified client side programming.  No _ws object exposed to
//              confuse novices, yet still retain advanced multi-server access
//              capability.  
//          * Only one client get(), set() method.  set() is now overloaded
//              and has far more options of restricting what is collected and
//              set on the server.
//          * Histogram re-written into a class so multiple charts can reside
//              on one web page.
//          * Histogram retains data through re-connects.  No longer clears on
//              reconnect.
//          * Histogram MIN, MAX functions added to the default averaging when 
//              multiple point accumulation is used.
//          * Exporting of Histogram data to CSV file for use with Excel, etc.
//          * Context sensative help on Admin pointing to web pages.
//              https://inqonthat.com/inqportal-help/
//          * Generation of a working starter HTML page using Sketch's 
//              published variables.  
// 5.0.1    2022/3/7
//          * library.properties corrections
//          * Removed some diagnostics that slipped through.
//          * Comma delimiters in Data Loss field.
// 5.1.0    2022/3/8
//          * Added functionality to permit publishRW set() methods to be
//              used as events no variable defined.
//          * Added ButtonClickEvent Example

// -----------------------------------------------------------------------------
// onInterval dependencies

#define PAUSE        0  // Sets it so it won't fire
#define DELETE (u32)-1  // Removes it from the list

typedef void (*FuncInterval)(void*);

// -----------------------------------------------------------------------------

class InqPortal
{
public:
    // All public methods are described:
    // https://inqonthat.com/inqportal-class-reference/
    
    // Creates instance of InqPortal.  All values have workable defaults.
    InqPortal(
        // User supplied buffer that will contain persisted data.  Typically
        // this is a supplied struct so data can easily use and update multiple
        // variables.
        u8* config = NULL, 
        
        // Size of supplied buffer (struct)
        u16 sizeConfig = 0, 
        
        // Speed for output to Arduino Serial Monitor.
        u32 serialBaud = 115200);
        
    // Starts up the instance of InqPortal.  All values have workable defaults.
    rc begin(
        // Default SSID published from the built-in router.  This is
        // configurable by the Admin, so it does not have to be defined.
        // If not defined, the SSID will be InqPortal-Xxxxx.
        // If defined, it can be overridden by the Admin.
        const char* defaultSoftSSID = NULL, 
        
        // Default password for the built-in router.  This is
        // configurable by the Admin, so it does not have to be defined.
        // The password can be blank, but if defined, it must have 8 or more
        // characters.
        const char* defaultSoftPW = NULL, 
        
        // Default connection information for your router.  These are 
        // configurable by the Admin and are only available here for your
        // use so you do not have to configure using the Admin.
        const char* defaultStationSSID = NULL,
        const char* defaultStationPW = NULL,

        // When connected to your router, this will be the computer name 
        // given to the router usable on your network.  If not defined,
        // the Soft AP SSID will be used.
        const char* defaultHostName = NULL, 
        
        // HTTP port used by the Web Server.  All browsers use port 80 by 
        // default.
        u16 port = 80);
        
    // Save persist data
    void saveConfig();

    //{ Publishing properties --------------------------------------------------
    
    // Add a heading line to the Admin for grouping your published variables
    void heading(const char* webID, const char* lbl);
    
    // The publish overloads publish a variable that can be seen and/or 
    // edited by your browser clients.  The overloads handle for different
    // types of variables.  They do not have to be tied to a variable, but
    // can trigger get and/or set user methods.  All overloads have the
    // same basic form.
    
    // In the expanded prototypes below, the following variable exist.
    // * webID = ID used in <div>, <span>, <input>... web elements.
    // * address = memory address where your global / static variable resides.
    //      This must be a permanent location during.
    // * size = Only used for buffer length.
    // * lbl = Your variables can be displayed and/or edited within the
    //      InqPortal Admin.  This is a friendly name to identify your 
    //      variable.  If set to NULL, it will not be displayed in the Admin,
    //      but still usable by your custom web pages.
    // * get = An optional callback method so you can return a value instead
    //      of having InqPortal just grab the value from the variable...
    //      say you want to calculate something when a browser makes the 
    //      request.
    // * set = An optional callback method so you can validate the value before
    //      storing.  Also can be used to trigger an event without the need
    //      of making a variable and supply its "address".  In this case,
    //      set address = NULL.
    
    // Read-only properties are ones that the browser clients can read, but
    // not update.  The Sketch can still change.
    
    // u8 variables
    void publishRO(const char* webID, u8* address, const char* lbl = NULL,
        u8(*get)() = NULL);
    void publishRW(const char* webID, u8* address, const char* lbl = NULL,
        u8(*get)() = NULL, void(*set)(u8) = NULL);

    // u16 variables
    void publishRO(const char* webID, u16* address, const char* lbl = NULL,
        u16(*get)() = NULL);
    void publishRW(const char* webID, u16* address, const char* lbl = NULL,
        u16(*get)() = NULL, void(*set)(u16) = NULL);

    // u32 variables
    void publishRO(const char* webID, u32* address, const char* lbl = NULL,
        u32(*get)() = NULL);
    void publishRW(const char* webID, u32* address, const char* lbl = NULL,
        u32(*get)() = NULL, void(*set)(u32) = NULL);

    // s8 variables
    void publishRO(const char* webID, s8* address, const char* lbl = NULL,
        s8(*get)() = NULL);
    void publishRW(const char* webID, s8* address, const char* lbl = NULL,
        s8(*get)() = NULL, void(*set)(s8) = NULL);

    // s16 variables
    void publishRO(const char* webID, s16* address, const char* lbl = NULL,
        s16(*get)() = NULL);
    void publishRW(const char* webID, s16* address, const char* lbl = NULL,
        s16(*get)() = NULL, void(*set)(s16) = NULL);

    // s32 variables
    void publishRO(const char* webID, s32* address, const char* lbl = NULL,
        s32(*get)() = NULL);
    void publishRW(const char* webID, s32* address, const char* lbl = NULL,
        s32(*get)() = NULL, void(*set)(s32) = NULL);

    // float variables
    void publishRO(const char* webID, float* address, const char* lbl = NULL,
        float(*get)() = NULL);
    void publishRW(const char* webID, float* address, const char* lbl = NULL,
        float(*get)() = NULL, void(*set)(float) = NULL);

    // double variables
    void publishRO(const char* webID, double* address, const char* lbl = NULL,
        double(*get)() = NULL);
    void publishRW(const char* webID, double* address, const char* lbl = NULL,
        double(*get)() = NULL, void(*set)(double) = NULL);

    // character array variables
    void publishRO(const char* webID, char* address, u16 length, 
        const char* lbl = NULL, char*(*get)() = NULL);
    void publishRW(const char* webID, char* address, u16 length, 
        const char* lbl = NULL, char*(*get)() = NULL, void(*set)(char*) = NULL);
   
    //} Publishing properties --------------------------------------------------

    // Sending out information from your server.
    // autoSend() sets up a recurring send of data.  
    // Setting interval = 0, turns it off.  
    // sendAll = true -> send all your published variables at each interval.
    // sendAll = false -> sends only the variables that have changed.
    // InqPortal sets this by default with Sketch doesn't (1000, false)
    void autoSend(u32 interval, bool sendAll = false);
    // Send all your published variables.
    void sendAll();
    // Send all your published variables that have changed.
    void sendChanged();
    // Send specific variables (published or not) now.
    bool send(const char* format, ...);
    
    // Send some log information that will go to the Admin log page.
    void LOG(u8 level, const char* format, ...);
    
    // Setup/modify schedules
    void onInterval(FuncInterval funcInterval, u32 interval, void* tag = NULL);
    
private:
    void validPublish(const char* webID);
    bool checkLabels(const char* format, va_list args);
};

#endif // INQ_PORTAL
