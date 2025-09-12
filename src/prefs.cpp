#include "config.h"
#include <Preferences.h>

Preferences prefs;

void
initWifiPrefs ()
{
  prefs.begin ("wifiCredentials", false);
}

void
initPincodePrefs ()
{
  prefs.begin ("pincode", false);
}

void
savePincode (char pincode[4])
{
  prefs.putBytes ("pincode", (byte *)(pincode), 4);
}

void
initTriesLeftPrefs()
{
  prefs.begin ("triesLeft", false);
}

void
saveTriesLeft(int triesLeft)
{
  prefs.putBytes ("triesLeft", &triesLeft, 4);
}

void getTriesLeft(int *triesLeft)
{
  prefs.getBytes ("triesLeft", triesLeft, 4);
}

void unsetPincode() {
  prefs.remove("pincode");
}

bool
isPincodeSet ()
{
  if (!prefs.isKey ("pincode"))
    {
      return false;
    }
  return true;
}

void
getPincode (char pincode[4])
{
  prefs.getBytes ("pincode", pincode, 4);
}

void
saveWiFiCredentials (String ssid, String password)
{
  prefs.putString ("ssid", ssid);
  prefs.putString ("password", password);
}

bool
getWiFiCredentials (String &ssid, String &password)
{
  ssid = prefs.getString ("ssid", "");
  password = prefs.getString ("password", "");
  return !ssid.isEmpty () && !password.isEmpty ();
}