#pragma once

class Panel : public USBMultiSerial<1>
{

    std::vector<std::vector<int>> keys = {
        // {PB7, '0'},
        {PB6, '1'},
        {PB5, '2'},
        {PB4, '3'},
        {PA15, '4'},
        {PA10, '5'},
        {PA9, '6'},
        {PA8, '7'},
        {PB15, '8'},
        {PB14, '9'},
        {PB13,'a'},
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
    void setup()
    {
        for (auto pin : keys)
        {
            pinMode(pin[0], INPUT_PULLUP);
        }
    }

    void loop()
    {

        for (size_t i = 0; i < keys.size(); i++)
        {
            if (digitalRead(keys[i][0]) == LOW)
            {
                ports[0].write(keys[i][1]);
                // ports[0].print(analogRead(PA3));
                // ports[0].print(' ');
                // ports[0].println(analogRead(PA2));
            }
        }
    }
};