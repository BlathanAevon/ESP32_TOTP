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
  Wire.begin ();
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

  if (allocateDisplay ())
    {
      Serial.println (F ("SSD1306 allocation failed"));
      for (;;)
        ;
    }

  setupFont ();
  setupInputs ();

  if (digitalRead (BUTTON_UP_PIN) == HIGH)
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

      initPincodePrefs ();
      initTriesLeftPrefs ();

      while (!isPincodeSet ())
        {
          displayFirstBootScreen ();
        };

      while (isAccessDenied ())
        {
          int triesLeft;
          getTriesLeft (&triesLeft);
          if (triesLeft == 0)
            {
              wipeServices ("/services.json");
              unsetPincode ();
              while (1)
                {
                  displayResetScreen ();
                }
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
}

void
loop ()
{
  if (isSetup)
    {
      processRequests ();
      displaySetupModeScreen ();
    }
  else
    {
      displayTotpScreen ();
    }
}
