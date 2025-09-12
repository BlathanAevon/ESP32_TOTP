#include <Arduino.h>
#include <RTClib.h>

#pragma once

#define BUTTON_UP_PIN 1
#define BUTTON_DOWN_PIN 2
#define BUTTON_OK_PIN 0
#define BUTTON_CANCEL_PIN 3
#define ADC_PIN 4

struct Service
{
  char name[64];
  char secret[32];
};

struct WiFiNetwork
{
  String ssid;
  String psk;
};

bool readServices (char *path);
bool writeServices (char *path, uint8_t *data);
bool allocateDisplay ();
bool beginRTC ();
bool isAccessDenied ();
bool isFirstBoot ();
bool checkPincode (char *correctPincode);
bool setupWifi (String ssid, String psk);
bool getWiFiCredentials (String &ssid, String &password);
bool setRTCtime ();
bool getIsRTCsynced ();
bool isPincodeSet ();

DateTime getCurrTime ();
Service *getServices ();

void setupAP ();
void setupDNS ();
void setupFont ();
void setupInputs ();
void setupServer ();
void wipeServices (char *path);
void updateCode (char *secret);
void displayBatteryState ();
void displayPincodeTries (int triesLeft);
void displayPincodeMarkers ();
void displayLockScreen ();
void displayTotpScreen ();
void displaySetupModeScreen ();
void displayResetScreen ();
void displayFirstBootScreen ();
void displayMsg (char *msg, int y);
void displayServicesErr ();
void readLockScreenInputs ();
void readTotpScreenInputs ();
void processRequests ();
void beginTimeClient ();
void initWifiPrefs ();
void initPincodePrefs ();
void initTriesLeftPrefs();
void saveWiFiCredentials (String ssid, String password);
void savePincode (char pincode[4]);
void saveTriesLeft(int triesLeft);
void getTriesLeft(int *triesLeft);
void getPincode (char pincode[4]);
void unsetPincode ();

int getCurrService ();
int getArraySize ();
int getCurrPincodeElem ();

char *getCode ();
char *getCurrPincode ();

static const char *apName = "notSensitiveDevice";
static const char *apPsk = "strongPassword123456";
