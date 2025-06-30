#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/*DISPLAY AND FONT SETUP*/
Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

void
setupFont ()
{
  u8g2_for_adafruit_gfx.setFontMode (1);
  u8g2_for_adafruit_gfx.setFontDirection (0);
  u8g2_for_adafruit_gfx.setForegroundColor (WHITE);
  u8g2_for_adafruit_gfx.setFont (u8g2_font_VCR_OSD_mr);
}

void
displayBatteryState ()
{
  int start = 39;
  int size = 5;
  int space = 5;
  for (int i = 0; i <= 4; i++)
    {
      display.fillRect (start, 59, size, size, WHITE);
      start += size + space;
    }
}

void
displayPincodeTries (int triesLeft)
{
  int start = 48;
  int size = 8;
  int space = 5;
  for (int i = 0; i <= triesLeft - 1; i++)
    {
      display.fillRect (start, 10, size, size, WHITE);
      start += size + space;
    }
}

bool
allocateDisplay ()
{
  if (!display.begin (SSD1306_SWITCHCAPVCC, 0x3C))
    {
      return true;
    }

  u8g2_for_adafruit_gfx.begin (display);
  return false;
}

void
displayPincodeMarkers ()
{
  int start = 5;
  int end = 30;
  int space = 5;
  int width = 25;
  for (int i = 0; i <= 3; i++)
    {
      if (i == getCurrPincodeElem ())
        {
          display.fillRect (start, 40, width, 3, WHITE);
        }
      else
        {
          display.drawLine (start, 40, end, 40, WHITE);
        }

      u8g2_for_adafruit_gfx.setCursor (
          start + u8g2_for_adafruit_gfx.getUTF8Width ("0") / 2, 38);
      u8g2_for_adafruit_gfx.println (getCurrPincode ()[i]);
      start = end + space;
      end += width + space;
    }
}

void
displayServicesErr ()
{
  display.clearDisplay ();
  displayMsg ("do setup", 20);
  displayMsg ("in web", 40);
  display.display ();
}

void
displayLockScreen ()
{
  readLockScreenInputs ();
  display.clearDisplay ();
  displayPincodeMarkers ();
  displayPincodeTries (getTriesLeft ());
  displayBatteryState ();
  display.display ();
}

void
displayFirstBootScreen ()
{
  readLockScreenInputs ();
  display.clearDisplay ();
  displayMsg ("SETUP", 20);
  displayPincodeMarkers ();
  display.display ();
}

void
displayTotpScreen ()
{
  DateTime now = getCurrTime ();

  int timer = (30 - (now.second () % 30)) * 4;
  int currService = getCurrService ();
  Service *services = getServices ();

  if (timer == 120)
    {
      updateCode (services[currService].secret);
    }

  readTotpScreenInputs ();

  display.clearDisplay ();
  u8g2_for_adafruit_gfx.setCursor (
      SCREEN_WIDTH / 2
          - (u8g2_for_adafruit_gfx.getUTF8Width (services[currService].name)
             / 2),
      20);
  u8g2_for_adafruit_gfx.println (services[currService].name);

  u8g2_for_adafruit_gfx.setCursor (
      SCREEN_WIDTH / 2 - (u8g2_for_adafruit_gfx.getUTF8Width ("000000") / 2),
      50);
  u8g2_for_adafruit_gfx.print (getCode ());

  display.drawRect (4, 60, timer, 4, WHITE);
  display.display ();
}

void
displayMsg (char *msg, int y)
{
  u8g2_for_adafruit_gfx.setCursor (
      SCREEN_WIDTH / 2 - (u8g2_for_adafruit_gfx.getUTF8Width (msg) / 2), y);
  u8g2_for_adafruit_gfx.println (msg);
}