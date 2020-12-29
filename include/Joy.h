#pragma once
#include <Arduino.h>
#include <USBComposite.h>
#include <vector>
#include "AnalogValue.h"
#include <cmath>
#include "common.h"

const auto DATA_SIZE = sizeof(JoystickReport_t) - 1;


class Joy : public HIDJoystick
{
private:
public:
    std::vector<AnalogValue> axes;
    std::vector<int> button_pins;

    Joy(USBHID &_HID, std::vector<AnalogValue>&& axes, std::vector<int>&& button_pins)
        : HIDJoystick(_HID), axes(axes), button_pins(button_pins)
    {
    }

    void setup()
    {
        for (auto pin : button_pins)
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
        auto s = (1023 - axes[5].get()) / 1023;

        joyReport.x = scale(1023 - axes[0].get(), s);
        joyReport.y = scale(axes[1].get(), s);
        joyReport.rx = scale(1023 - axes[2].get(), s);
        joyReport.ry = scale(axes[3].get(), s);

        joyReport.sliderLeft = 1023 - axes[4].get();

        joyReport.buttons = 0;
        for (size_t i = 0; i < button_pins.size(); i++)
        {
            if (button_pins[i] != DISABLED)
            {
                joyReport.buttons |= (0x1 & ~digitalRead(button_pins[i])) << i;
            }
        }

        sendReport();
    }
};