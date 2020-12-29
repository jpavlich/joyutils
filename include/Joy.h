#pragma once
#include <Arduino.h>
#include <USBComposite.h>
#include <vector>
#include "AnalogValue.h"
#include <cmath>
#include  "common.h"

const auto DATA_SIZE = sizeof(JoystickReport_t) - 1;

// -16852
// 10383

const float THROTTLE_MIN = (32767 - 16852) >> 4;
const float THROTTLE_MAX = (32767 + 10383) >> 4;

class Joy : public HIDJoystick
{
private:
public:
    std::vector<AnalogValue> values = {
        AnalogValue(PA0),
        AnalogValue(PA1),
        AnalogValue(PA2),
        AnalogValue(PA3),
        AnalogValue(PA4, THROTTLE_MIN, THROTTLE_MAX),
        AnalogValue(PA5),
    };
    std::vector<int> buttons = {
        // DISABLED,
        // DISABLED,       
        PB7,        
        // PB6,        
        // PB5,        
        // PB4,        
        // PA15,       
        // PA10,       
        // PA9,        
        // PA8,        
        // PB15,       
        // PB14,       
        // PB13,
        // PB12,
        // PB11,
        // PB10,
        // PB1,
        // PA7,
        // PA6,
        // PA5,
        // PB9,
        // PB8,
    };

    Joy(USBHID &_HID)
        : HIDJoystick(_HID)
    {
    }

    void setup()
    {
        for (auto pin : buttons)
        {
            pinMode(pin, INPUT_PULLUP);
        }
    }

    float scale(float val, float scale)
    {
        return (val - 511) * scale + 511;
    }

    void loop()
    {
        auto s = (1023 - values[5].get()) / 1023;

        joyReport.x = scale(1023 - values[0].get(), s);
        joyReport.y = scale(values[1].get(), s);
        joyReport.rx = scale(1023 - values[2].get(), s);
        joyReport.ry = scale(values[3].get(), s);

        joyReport.sliderLeft = 1023 - values[4].get();

        joyReport.buttons = 0;
        for (size_t i = 0; i < buttons.size(); i++)
        {
            if (buttons[i] != DISABLED)
            {
                joyReport.buttons |= (0x1 & ~digitalRead(buttons[i])) << i;
            }
        }

        sendReport();
    }
};