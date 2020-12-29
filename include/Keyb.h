#pragma once
#include <Arduino.h>
#include <USBComposite.h>
#include <vector>

const auto KP_0 = 0x62;
const auto KP_1 = 0x59;
const auto KP_2 = 0x5A;
const auto KP_3 = 0x5B;
const auto KP_4 = 0x5C;
const auto KP_5 = 0x97;
const auto KP_6 = 0x5E;
const auto KP_7 = 0x5F;
const auto KP_8 = 0x60;
const auto KP_9 = 0x61;

const auto WAIT_CYCLES = 1;

typedef struct
{
    uint8_t reportID;
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[1];
} __packed KeybReport;

class Keyb : public HIDReporter
{
    std::vector<std::vector<int>> keys = {
        {PB7, KP_0},
        {PB6, KP_1},
        {PB5, KP_2},
        {PB4, KP_3},
        {PA15, KP_4},
        {PA10, KP_5},
        {PA9, KP_6},
        {PA8, KP_7},
        {PB15, KP_8},
        {PB14, KP_9},
        // {PB13,KEY_},
        // {PB12,KEY_},
        // {PB11,KEY_},
        // {PB10,KEY_},
        // {PB1,KEY_},
        // {PA7,KEY_},
        // {PA6,KEY_},
        // {PA5,KEY_},
        // {PB9,KEY_},
        // {PB8,KEY_},
    };

public:
    KeybReport keyReport;

    Keyb(USBHID &HID, uint8_t _reportID = HID_KEYBOARD_REPORT_ID) : HIDReporter(HID, hidReportKeyboard, (uint8 *)&keyReport, sizeof(KeyReport_t), _reportID)                                                              
    {
    }
    void end(void);

    void begin()
    {
        for (auto pin : keys)
        {
            pinMode(pin[0], INPUT_PULLUP);
        }
    }

    void pressRawKey(uint8_t k)
    {
        keyReport.keys[0] = k;
        sendReport();
    }

    void loop()
    {
        static uint8_t cycle = 0;
        if (cycle++ % WAIT_CYCLES == 0)
        {

            for (size_t i = 0; i < keys.size(); i++)
            {
                if (digitalRead(keys[i][0]) == LOW)
                {
                    pressRawKey(keys[i][1]);
                }
                else
                {
                    pressRawKey(0);
                }
            }
        }
    }
};