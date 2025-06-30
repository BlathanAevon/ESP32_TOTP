#include "config.h"
#include <Arduino.h>

/*TOTP CONFIG */
int period = 1000;
unsigned long last_press = 0;

/* PINCODE CONFIG */
char pincodeSymbols[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
int currSymbol = 0;
int currService = 0;
int accessDenied = true;
int triesLeft = 3;
char currPincode[4] = { ' ', ' ', ' ', ' ' };
int currPincodeElem = 0;
char correctPincode[4];

bool
isAccessDenied ()
{
  return accessDenied;
}

bool
isFirstBoot ()
{
  return accessDenied;
}

void
setupInputs ()
{
  pinMode (BUTTON_UP_PIN, INPUT);
  pinMode (BUTTON_DOWN_PIN, INPUT);
  pinMode (BUTTON_OK_PIN, INPUT);
  pinMode (BUTTON_CANCEL_PIN, INPUT);
}

int
getCurrService ()
{
  return currService;
}

int
getCurrPincodeElem ()
{
  return currPincodeElem;
}

char *
getCurrPincode ()
{
  return currPincode;
}

int
getTriesLeft ()
{
  return triesLeft;
}

void
readTotpScreenInputs ()
{
  Service *services = getServices ();

  if (digitalRead (BUTTON_UP_PIN) == LOW && millis () - last_press > 200)
    {
      currService = (currService + 1) % getArraySize ();
      last_press = millis ();
      updateCode (services[currService].secret);
    }

  if (digitalRead (BUTTON_DOWN_PIN) == LOW && millis () - last_press > 200)
    {
      currService = (currService + 1) % getArraySize ();
      last_press = millis ();
      updateCode (services[currService].secret);
    }
}

bool
checkPincode (char *correctPincode)
{
  for (int i = 0; i <= 3; i++)
    {
      if (currPincode[i] != correctPincode[i])
        {
          triesLeft -= 1;
          currPincodeElem = 0;
          currSymbol = 0;
          for (int i = 0; i <= 3; i++)
            {
              currPincode[i] = ' ';
            }
          return false;
        }
    }
  return true;
}

void
readLockScreenInputs ()
{
  if (digitalRead (BUTTON_OK_PIN) == LOW && millis () - last_press > 200)
    {
      last_press = millis ();

      if (currPincode[currPincodeElem] != ' ')
        {
          if (currPincodeElem
              == sizeof (currPincode) / sizeof (currPincode[0]) - 1)
            {

              if (!isPincodeSet ())
                {
                  savePincode (currPincode);
                  for (int i = 0; i <= 3; i++)
                    {
                      currPincode[i] = ' ';
                    }
                  currPincodeElem = 0;
                  currSymbol = 0;
                  return;
                };

              getPincode (correctPincode);

              if (checkPincode (correctPincode))
                {
                  accessDenied = false;
                }
            }
          else
            {
              currPincodeElem += 1;
              currSymbol = 0;
            }
        }
    }
  if (digitalRead (BUTTON_CANCEL_PIN) == LOW && millis () - last_press > 200)
    {
      last_press = millis ();

      if (currPincodeElem != 0)
        {
          currSymbol = currPincodeElem - 1;
          currPincode[currPincodeElem] = ' ';
          currPincodeElem -= 1;
        }
    }
  if (digitalRead (BUTTON_UP_PIN) == LOW && millis () - last_press > 200)
    {
      last_press = millis ();
      if (currSymbol
          == sizeof (pincodeSymbols) / sizeof (pincodeSymbols[0]) - 1)
        {
          currSymbol = 0;
        }
      else
        {
          currSymbol += 1;
        }

      currPincode[currPincodeElem] = pincodeSymbols[currSymbol];
    }
  else if (digitalRead (BUTTON_DOWN_PIN) == LOW
           && millis () - last_press > 200)
    {
      last_press = millis ();
      if (currSymbol == 0)
        {
          currSymbol
              = sizeof (pincodeSymbols) / sizeof (pincodeSymbols[0]) - 1;
        }
      else
        {
          currSymbol -= 1;
        }

      currPincode[currPincodeElem] = pincodeSymbols[currSymbol];
    }
}