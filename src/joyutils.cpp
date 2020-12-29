#include <Arduino.h>
#include <USBComposite.h>
#include "Joy.h"
#include "KspIO.h"
#include "AnalogValue.h"
#include <vector>
#define LED_BUILTIN PB12
USBHID HID;
Joy joystick(HID);
KspIO kspIO;

void setup()
{

  USBComposite.clear();
  HID.registerComponent();
  kspIO.registerComponent();
  USBComposite.begin();
  joystick.setup();
  kspIO.setup();

  while (!USBComposite)
    ;
}

void loop()
{
  joystick.loop();
  kspIO.loop();
}
