#include <InqPortal.h>

InqPortal svr;

void setup() 
{
  svr.publishRW("ClkMe", NULL, NULL, NULL, clickedMe);
  svr.begin("SoftSSID", NULL, "<your SSID>", "<your Password>", "HostName");
}

void loop() 
{
  // put your main code here, to run repeatedly:
}

void clickedMe(u8 val)
{
  svr.LOG(1, "A person clicked the client's button (%u)\n", val);
}
