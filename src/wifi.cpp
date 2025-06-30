#include <Arduino.h>
#include <WiFi.h>

void
setupAP ()
{
  Serial.println ("STARTED AP");
  WiFi.mode (WIFI_MODE_APSTA);
  WiFi.softAP ("ESP32_TOTP", "12345678", 1, 0, 1);
}

bool
setupWifi (String ssid, String psk)
{
  int connectionTries = 0;

  if (ssid != "" && psk != "")
    {
      Serial.println ("wifi settings were saved before");
      Serial.printf ("Loaded: %s %s\n", ssid, psk);

      WiFi.begin (ssid, psk);

      while (WiFi.status () != WL_CONNECTED || connectionTries > 50)
        {
          Serial.print (".");
          delay (100);
          connectionTries++;

          if (connectionTries >= 50)
            {
              Serial.println ("\nFailed connection to WiFi network");
              return false;
            }
          else
            {
              Serial.println ("\nConnected to the WiFi network");
              return true;
            }
        }
    }

  Serial.println ("no previously saved wifi configuration found. connect via "
                  "web interface");
  return false;
}