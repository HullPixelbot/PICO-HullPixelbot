#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESPmDNS.h>
#define MDNS_ID "esp32"
#endif

#ifdef ARDUINO_ARCH_PICO
#include <LEAmDNS.h>
#define MDNS_ID "pico"
#endif

#include "Pixels.h"
#include "WebPage.h"

#include "Commands.h"

const char *ssid = "ZyXEL56E8A7";
const char *password = "DAB6E5A9EC25";

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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting WiFi");

  startBusyPixel(0, 255, 255);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    updateBusyPixel();
    delay(500);
  }

  stopBusyPixel();

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(MDNS_ID))
  {
    Serial.printf("MDNS responder started on %s\n",MDNS_ID);
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
}

void updateServer()
{
  server.handleClient();
#ifdef ARDUINO_ARCH_PICO
  MDNS.update();
#endif
}
