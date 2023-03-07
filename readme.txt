InqPortal Library

ESP8266 Full featured IoT Application Server.

IMPORTANT:  To compile using the InqPortal library, you must add the 
    platform.local.txt file included in this folder to your ESP8266 library
    folder.  
    
    If you have a standard installation using the Windows installer, you 
    should find the location at YMMV:
    C:\Users\<User>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2
    
    If you followed our on-line installation using a Portable Arduino IDE, 
    you should find the proper location at YMMY:    
    C:\Users\<User>\Documents\Arduino\arduino-1.8.19\portable\packages\esp8266\hardware\esp8266\3.0.2
    
Derived directly from lwIP libraries to support application development.
It includes a full browser based Admin GUI to
  (1) manage Soft and multiple Station Access Points with full scan for 
    stations, and fail over like any good smartphone or laptop. 
  (2) Dedicated File management using an optimized file system supporting 
    flash memory wear leveling (Not SPIFFs or LittleFS).  Management of the
    file system via Admin mimicking Windows File Explorer with Drag&Drop.  
  (3) Performance metrics to monitor IoT server application and extensions
    beyond the simple logging of Serial Monitor.  Watch variables and logging
    supports runtime disabling and remote viewing of messages from the 
    browser Admin. 
  (4) Customizable, Windows style Task Manager type histogram that can also 
    be used for IoT application custom published variables.  
  (5) Custom high-performance API using WebSockets for responsive and 
    high-rate communications between browser clients and ESP8266 IoT server.
  (6) Admin provides client Starter App generation to kick-start your custom
    end-user facing GUI.  Use the built-in Chart.js for graphing or drop in
    your favorite browser based graphics libraries like dashboards, dials 
    and gauges.
    
All documentation is on-line
    
* https://InqOnThat.com/InqPortal - Quick Start, Tutorial, Sample Projects
* https://inqonthat.com/inqportal-class-reference/ - Sketch/C++ Library Reference
* https://inqonthat.com/inqportal-help/ (TBD) - Admin context sensative help links.  
