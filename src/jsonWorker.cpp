#include "config.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

struct Service services[100];
int arraySize;

int
getArraySize ()
{
  return arraySize;
}

Service *
getServices ()
{
  return services;
}

bool
readServices (char *path)
{
  JsonDocument doc;
  File file = LittleFS.open (path, "r");

  if (deserializeJson (doc, file))
    {
      return true;
    }

  JsonArray array = doc.as<JsonArray> ();
  int counter = 0;

  arraySize = array.size ();

  for (JsonObject obj : array)
    {
      struct Service service = {};

      const char *name = obj["name"] | "";
      const char *secret = obj["secret"] | "";

      strncpy (service.name, name, sizeof (service.name));
      strncpy (service.secret, secret, sizeof (service.secret));

      services[counter] = service;
      counter++;
    }

  file.close ();
  return false;
}

void
wipeServices (char *path)
{
  File file = LittleFS.open (path, "w");
  file.write ({});
  file.close ();
}

bool
writeServices (char *path, uint8_t *data)
{
  JsonDocument doc;

  if (deserializeJson (doc, data))
    {
      Serial.println ("Failed to parse JSON");
      return true;
    }

  JsonArray array = doc.as<JsonArray> ();
  int counter = 0;

  for (JsonObject obj : array)
    {

      struct Service service = {};

      const char *name = obj["name"] | "";
      const char *secret = obj["secret"] | "";

      strncpy (service.name, name, sizeof (service.name));
      strncpy (service.secret, secret, sizeof (service.secret));

      services[counter] = service;
      counter++;
    }

  File file = LittleFS.open (path, "w");
  serializeJson (doc, file);
  file.close ();

  return false;
}