#include "ESPAsyncWebServer.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <Preferences.h>

DNSServer dnsServer;
AsyncWebServer server (80);

struct WiFiNetwork chosenNetwork;
String wifiNetworksJsonString;
String connectedNetworkString;
JsonDocument wifiNetworksJson;
JsonDocument connectedNetworkJson;
JsonDocument servicesJson;

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler () {}
  virtual ~CaptiveRequestHandler () {}

  bool
  canHandle (AsyncWebServerRequest *request)
  {
    String path = request->url ();
    if (path == "/" || path == "/style.css" || path == "/index.js"
        || path == "/startWifiScan" || path == "/getWifiScan"
        || path == "/connectToNetwork" || path == "/isConnected"
        || path == "/saveWifiSettings" || path == "/syncRTC"
        || path == "/disconnectFromNetwork" || path == "/getServices"
        || path == "/saveServices" || path == "/isRTCsynced")
      {
        return false;
      }
    return true;
  }

  void
  handleRequest (AsyncWebServerRequest *request)
  {
    request->send (LittleFS, "/index.html", "text/html", false);
  }
};

void
setupDNS ()
{
  Serial.println ("SETUP DNS");
  dnsServer.start (53, "*", WiFi.softAPIP ());
  server.addHandler (new CaptiveRequestHandler ()).setFilter (ON_AP_FILTER);
  server.begin ();
}

void
processRequests ()
{
  dnsServer.processNextRequest ();
}

void
setupServer ()
{
  Serial.println ("SETUP SERVER");

  server.on ("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send (LittleFS, "/index.html", "text/html", false);
  });

  server.on ("/style.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send (LittleFS, "/style.css", "text/css", false);
  });

  server.on ("/index.js", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send (LittleFS, "/index.js", "text/javascript", false);
  });

  server.on ("/startWifiScan", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/startWifiScan triggered");
    WiFi.scanNetworks (true);
    request->send (200, "text/plain", "OK");
  });

  server.on ("/getWifiScan", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/getWiFiScan triggered");
    int n = WiFi.scanComplete ();
    if (n >= 0)
      {
        Serial.printf ("%d network(s) found\n", n);
        struct WiFiNetwork networks[n];

        for (int i = 0; i < n; ++i)
          {
            networks[i] = WiFiNetwork{ WiFi.SSID (i), String (WiFi.RSSI (i)) };
            wifiNetworksJson[i]["id"] = i;
            wifiNetworksJson[i]["SSID"] = WiFi.SSID (i);
            wifiNetworksJson[i]["RSSI"] = WiFi.RSSI (i);
          }

        WiFi.scanDelete ();

        serializeJson (wifiNetworksJson, wifiNetworksJsonString);

        request->send (200, "application/json", wifiNetworksJsonString);
      }
    request->send (200, "text/plain", "scan not finished");
  });

  server.on (
      "/connectToNetwork", HTTP_POST, [] (AsyncWebServerRequest *request) {},
      NULL,
      [] (AsyncWebServerRequest *request, uint8_t *data, size_t len,
          size_t index, size_t total) {
        Serial.println ("/connectToNetwork triggered");

        JsonDocument doc;
        if (deserializeJson (doc, data))
          {
            Serial.println ("Failed to parse JSON");
            request->send (200, "text/plain", "FAIL");
            return;
          }

        chosenNetwork.ssid = doc["ssid"] | "";
        chosenNetwork.psk = doc["psk"] | "";

        WiFi.begin (chosenNetwork.ssid, chosenNetwork.psk);

        request->send (200, "text/plain", "OK");
      });

  server.on ("/isConnected", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/isConnected triggered");
    if (WiFi.status () == WL_CONNECTED)
      {

        connectedNetworkJson["success"] = true;
        connectedNetworkJson["ssid"] = chosenNetwork.ssid == ""
                                           ? String (WiFi.SSID ())
                                           : chosenNetwork.ssid;

        serializeJson (connectedNetworkJson, connectedNetworkString);

        request->send (200, "application/json", connectedNetworkString);
      }
    if (WiFi.status () == WL_CONNECT_FAILED
        || WiFi.status () == WL_DISCONNECTED)
      {
        connectedNetworkJson["success"] = false;
        connectedNetworkJson["ssid"] = "";

        serializeJson (connectedNetworkJson, connectedNetworkString);

        request->send (200, "application/json", connectedNetworkString);
      }
    request->send (200, "text/plain", "not connected");
  });

  server.on ("/saveWifiSettings", HTTP_GET,
             [] (AsyncWebServerRequest *request) {
               Serial.println ("/saveWifiSettings triggered");

               saveWiFiCredentials (chosenNetwork.ssid, chosenNetwork.psk);

               request->send (200, "text/plain", "OK");
             });

  server.on ("/syncRTC", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/syncRTC triggered");

    setRTCtime ();

    request->send (200, "text/plain", "OK");
  });

  server.on ("/disconnectFromNetwork", HTTP_GET,
             [] (AsyncWebServerRequest *request) {
               Serial.println ("/disconnectFromNetwork triggered");

               WiFi.disconnect (false, false);

               Serial.println ("wifi disconnected");

               request->send (200, "text/plain", "OK");
             });

  server.on ("/isRTCsynced", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/isRTCsynced triggered");

    if (getIsRTCsynced ())
      {
        request->send (200, "text/plain", "SYNCED");
      }
    else
      {
        request->send (200, "text/plain", "NOT SYNCED");
      }
  });

  server.on ("/getServices", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println ("/getServices triggered");

    File file = LittleFS.open ("/services.json", "r");

    request->send (200, "application/json", file.readString ());
    file.close ();
  });

  server.on (
      "/saveServices", HTTP_POST, [] (AsyncWebServerRequest *request) {}, NULL,
      [] (AsyncWebServerRequest *request, uint8_t *data, size_t len,
          size_t index, size_t total) {
        Serial.println ("/saveServices triggered");

        if (writeServices ("/services.json", data))
          {
            Serial.println ("Failed to parse JSON");
            request->send (200, "text/plain", "FAIL");
            return;
          }

        Serial.println ("Services saved");

        request->send (200, "text/plain", "OK");
      });
}