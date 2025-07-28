#include "config.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <WiFi.h>

bool isSetup = false;

void
setup ()
{
  delay (1000);
  Serial.begin (115200);

  if (beginRTC ())
    {
      Serial.println ("DS3231 Allocation failed");
      while (1)
        ;
    }

  if (!LittleFS.begin (true))
    {
      Serial.println ("LittleFS Mount Failed");
      while (1)
        ;
    }

  setupInputs ();

  if (digitalRead (BUTTON_UP_PIN) == LOW)
    {
      Serial.println ("CONFIG SETUP MODE");
      isSetup = true;

      initWifiPrefs ();

      String ssid;
      String psk;

      getWiFiCredentials (ssid, psk);

      setupAP ();
      setupDNS ();
      beginTimeClient ();
      if (setupWifi (ssid, psk))
        {
          if (!setRTCtime ())
            {
              Serial.println ("COULD NOT SET RTC TIME");
            };
          WiFi.disconnect (false, false);
        }
      setupServer ();
    }
  else
    {
      Serial.println ("MAIN MODE");
      Wire.begin ();

      if (allocateDisplay ())
        {
          Serial.println (F ("SSD1306 allocation failed"));
          for (;;)
            ;
        }

      setupFont ();
      initPincodePrefs ();

      while (!isPincodeSet ())
        {
          displayFirstBootScreen ();
        };

      while (isAccessDenied ())
        {
          if (getTriesLeft () == 0)
            {
              wipeServices ("/services.json");
              unsetPincode ();
              Serial.println (
                  "PASSWORD ENTERED WRONG 3 TIMES. SERVICES WIPED!");
              break;
            }
          displayLockScreen ();
        }

      if (readServices ("/services.json"))
        {
          Serial.println ("Failed to parse JSON");
          displayServicesErr ();
          for (;;)
            ;
        }
    }

  updateCode (getServices ()[getCurrService ()].secret);

  Serial.println ("main setup() complete");
}

void
loop ()
{
  if (isSetup)
    {
      processRequests ();
    }
  else
    {
      displayTotpScreen ();
    }
}
