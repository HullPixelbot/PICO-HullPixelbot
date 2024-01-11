#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "Storage.h"

#ifdef ARDUINO_ARCH_ESP32
#include <ESPmDNS.h>
#define MDNS_ID "esp32"
#endif

#ifdef ARDUINO_ARCH_PICO
#include <LEAmDNS.h>
#endif

#include "Pixels.h"
#include "WebPage.h"

#include "Commands.h"

bool serverActive = false;

WebServer server(80);

void handleRoot()
{
  server.send(200, "text/html", webPage);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setupServer()
{

  char ssid [NETWORK_SETTING_LENGTH+1];
  char password [NETWORK_SETTING_LENGTH+1];
  char name [NETWORK_SETTING_LENGTH+1];

  loadBlockFromEEPROM((uint8_t *)ssid,NETWORK_SETTING_LENGTH+1,WIFI_NETWORK_SSID_OFFSET);
  loadBlockFromEEPROM((uint8_t *)password,NETWORK_SETTING_LENGTH+1,WIFI_NETWORK_PASSWORD_OFFSET);
  loadBlockFromEEPROM((uint8_t *)name,NETWORK_SETTING_LENGTH+1,WIFI_NETWORK_NETWORK_NAME_OFFSET);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting WiFi");

  startBusyPixel(0, 255, 255);

  int tickCount = 0;

  // Wait for connection
  // Try for ten seconds and then error out

  while (WiFi.status() != WL_CONNECTED)
  {
    updateBusyPixel();

    delay(500);

    if(++tickCount>20){
      displayBusyPixelWait(20,200,255,0,0);
      stopBusyPixel();
      return;
    }
  }

  stopBusyPixel();

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if(name[0]==0)
  {
    strcpy(name,"pico");
  }

  if (MDNS.begin(name))
  {
    Serial.printf("MDNS responder started on %s\n", name);
  }

  server.on("/", handleRoot);

  server.on("/run", []()
            {
  Serial.println("Got a run request");
  sendLineToRobot("*RS");
  handleRoot(); });

  server.on("/stop", []()
            {
//    Serial.println("Got a stop request");
    sendLineToRobot("*RH");
    handleRoot(); });

  server.on("/save", []()
            {
    // code is the only argument
    String robotCode = server.arg(0); 
    sendLineToRobot("begin");
    sendStringToRobot(robotCode);
    sendLineToRobot("end");
    handleRoot(); });

  server.onNotFound(handleNotFound);

  server.begin();
  serverActive = true;
}

void updateServer()
{
  if (serverActive)
  {
    server.handleClient();
#ifdef ARDUINO_ARCH_PICO
    MDNS.update();
#endif
  }
}
