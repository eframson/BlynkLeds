/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

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

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

int redVal=0;
int blueVal=0;
int greenVal=0;
int ledSetIdx=0;
int ledPatternLength=0;
int ledBrightness=255;
bool delayShow=false;
LinkedList<CRGB> ledPattern = LinkedList<CRGB>();

CRGBArray<NUM_LEDS> leds;

BlynkTimer timer;

void ClearDisplay()
{
  delayShow=true;
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
      leds[dot] = CRGB::Black;
  }
  for(int i = ledPatternLength - 1; i > -1; i--){
    ledPattern.remove(i);
  }
  //ledPattern.clear();
  ledPatternLength = ledPattern.size();
  Blynk.virtualWrite(V1, ledPatternLength);
  delayShow=false;
}

CRGB GetRGB(int r, int g, int b) {
  return CRGB(r,g,b);
}

void SetPixel(bool doClear=false)
{
  if(doClear){
    //leds[ledSetIdx] = CRGB::Black;
  } else {
    if(ledSetIdx >= ledPatternLength){
      ledPattern.add(GetRGB(redVal, greenVal, blueVal));
    } else {
      ledPattern.set(ledSetIdx, GetRGB(redVal, greenVal, blueVal));
    }
  }
  ledPatternLength = ledPattern.size();
  Blynk.virtualWrite(V1, ledPatternLength);
  applyPatternToString();
}

void applyPatternToString(){
  delayShow=true;
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = ledPattern.get(i % ledPatternLength);
  }
  delayShow=false;
}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
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
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  Blynk.virtualWrite(V2, millis() / 1000);

  if(delayShow == false){
    FastLED.setBrightness(ledBrightness);
    FastLED.show();
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setTemperature(Tungsten40W);  // GRB ordering is typical
  FastLED.setBrightness(ledBrightness);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  //Is this better or worse than EVERY_N_MILLISECONDS?
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
