// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLZdpa9Dvn"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "W_UT-tlwiB2ii8JGdVx7X18lI_KgOX5t"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <LinkedList.h>
#include <BlynkSimpleEsp32.h>
#include "FastLED.h"
#include "wifi_config.h"

#define NUM_LEDS 100
#define DATA_PIN 13
#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof(x[0]))
#define DEBUGMODE 1
#define BRIGHTNESS 255 // 1-255

char auth[] = BLYNK_AUTH_TOKEN;

const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

struct rgb {
  int r;
  int g;
  int b;
  rgb(){};
  rgb(int r, int g, int b)
    : r(r)
    , g(g)
    , b(b)
  {};
};

int redVal=0;
int blueVal=0;
int greenVal=0;
int ledSetIdx=0;
int ledPatternLength=0;
int ledBrightness=255;
bool delayShow=false;

LinkedList<rgb> ledPattern;
CRGBArray<NUM_LEDS> leds;
BlynkTimer timer;

void ClearDisplay()
{
  delayShow=true;
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
      leds[dot] = CRGB::Black;
  }
  ledPattern.clear();
  ledPatternLength = ledPattern.size();
  Blynk.virtualWrite(V1, ledPatternLength);
  delayShow=false;
}

rgb* GetRGB(int r, int g, int b) {
  return new rgb(r,g,b);
}

void SetPixel(bool doClear=false)
{
  if(doClear){
    //leds[ledSetIdx] = CRGB::Black;
  } else {
    if(ledSetIdx >= ledPatternLength){
      ledPattern.add(*GetRGB(redVal, greenVal, blueVal));
    } else {
      ledPattern.set(ledSetIdx, *GetRGB(redVal, greenVal, blueVal));
    }
  }
  
  ledPatternLength = ledPattern.size();
  Blynk.virtualWrite(V1, ledPatternLength);
  
  ApplyPatternToString();
}

void ApplyPatternToString(){
  delayShow=true;
  for (int i = 0; i <= NUM_LEDS; i++) {
    rgb thisPixel = ledPattern.get(i % ledPatternLength);
    leds[i] = CRGB(thisPixel.r, thisPixel.g, thisPixel.b);
  }
  delayShow=false;
}

BLYNK_WRITE(V0)
{
  int value = param.asInt();
  Blynk.virtualWrite(V1, ledPatternLength);
}

BLYNK_WRITE(V4)
{
  int value = param.asInt();
  redVal = value;
}

BLYNK_WRITE(V5)
{
  int value = param.asInt();
  blueVal = value;
}

BLYNK_WRITE(V6)
{
  int value = param.asInt();
  greenVal = value;
}

BLYNK_WRITE(V7)
{
  int value = param.asInt();
  ledSetIdx = value;
}

BLYNK_WRITE(V8)
{
  int value = param.asInt();
  if(value == 1){
    SetPixel();
  }
}

BLYNK_WRITE(V9)
{
  int value = param.asInt();
  if(value == 1){
    ClearDisplay();
  }
}

BLYNK_WRITE(V10)
{
  int value = param.asInt();
  if(value == 1){
    SetPixel(true);
  }
}

BLYNK_WRITE(V11)
{
  int value = param.asInt();
  ledBrightness = value;
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Serial.println("Connected to Blynk.Cloud!");
}

void DoEverySecond()
{
  UpdateUptime();

  if(delayShow == false){
    FastLED.setBrightness(ledBrightness);
    FastLED.show();
  }
}

void UpdateUptime()
{
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setTemperature(Tungsten40W);  // GRB ordering is typical
  FastLED.setBrightness(ledBrightness);

  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every second
  //Is this better or worse than EVERY_N_MILLISECONDS?
  timer.setInterval(1000L, DoEverySecond);
}

void loop()
{
  Blynk.run();
  timer.run();
}
