#include "config.h"
#include <Arduino.h>
#include <Base32-Decode.h>
#include <NTPClient.h>
#include <RTClib.h>
#include <TOTP.h>
#include <WiFi.h>

RTC_DS3231 rtc;
WiFiUDP ntpUDP;
NTPClient timeClient (ntpUDP, "pool.ntp.org", 0, 60000);
char codeBuffer[7];
bool isRTCsynced = false;

void
beginTimeClient ()
{
  timeClient.begin ();
}

bool
beginRTC ()
{
  if (!rtc.begin ())
    {
      return true;
    }
  return false;
}

DateTime
getCurrTime ()
{
  return rtc.now ();
}

bool
getIsRTCsynced ()
{
  return isRTCsynced;
}

bool
setRTCtime ()
{
  if (!timeClient.update ())
    {
      return false;
    };

  unsigned long epochTime = timeClient.getEpochTime ();
  DateTime ntpTime (epochTime);
  rtc.adjust (ntpTime);
  isRTCsynced = true;

  return true;
}

void
updateCode (char *secret)
{
  uint8_t decoded[32];
  int keyLen = base32decode (secret, decoded, sizeof (decoded));
  TOTP totp = TOTP (decoded, keyLen);
  strncpy (codeBuffer, totp.getCode (rtc.now ().unixtime ()),
           sizeof (codeBuffer));
  codeBuffer[6] = '\0';
}

char *
getCode ()
{
  return codeBuffer;
}