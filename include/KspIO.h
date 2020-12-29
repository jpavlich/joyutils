#pragma once
#include <USBComposite.h>
#include "packets.h"
#include <Arduino.h>
#include <cstdint>

// https://sites.google.com/site/zitronfiles/KSPIODemo17.zip

class KspIO : public USBMultiSerial<1>
{
public:
    boolean connected = false;

    void setup()
    {
        handshakePkt.id = 0;
        controlPkt.id = 101;
    }

    void loop()
    {
        input();
        output();
    }

protected:
    unsigned long deadtime, deadtimeOld, controlTime, controlTimeOld;
    unsigned long now;

    byte caution = 0, warning = 0, id;

    HandShakePacket handshakePkt;
    VesselData vesselData;
    ControlPacket controlPkt;

    uint8_t rx_len;
    uint16_t *address;
    byte buffer[256]; //address for temporary storage and parsing buffer
    uint8_t structSize;
    uint8_t rx_array_inx; //index for RX parsing buffer
    uint8_t calc_CS;      //calculated Chacksum

    //This contains stuff borrowed from EasyTransfer lib
    boolean KSPBoardReceiveData()
    {
        if ((rx_len == 0) && (ports[0].available() > 3))
        {
            while (ports[0].read() != 0xBE)
            {
                if (ports[0].available() == 0)
                    return false;
            }
            if (ports[0].read() == 0xEF)
            {
                rx_len = ports[0].read();
                id = ports[0].read();
                rx_array_inx = 1;

                switch (id)
                {
                case 0: // Received handshake
                    structSize = sizeof(handshakePkt);
                    address = (uint16_t *)&handshakePkt;
                    break;
                case 1: // received vessel data
                    structSize = sizeof(vesselData);
                    address = (uint16_t *)&vesselData;
                    break;
                }
            }

            //make sure the binary structs on both Arduinos are the same size.
            if (rx_len != structSize)
            {
                rx_len = 0;
                return false;
            }
        }

        if (rx_len != 0)
        {
            while (ports[0].available() && rx_array_inx <= rx_len)
            {
                buffer[rx_array_inx++] = ports[0].read();
            }
            buffer[0] = id;

            if (rx_len == (rx_array_inx - 1))
            {
                //seem to have got whole message
                //last uint8_t is CS
                calc_CS = rx_len;
                for (int i = 0; i < rx_len; i++)
                {
                    calc_CS ^= buffer[i];
                }

                if (calc_CS == buffer[rx_array_inx - 1])
                { //CS good
                    memcpy(address, buffer, structSize);
                    rx_len = 0;
                    rx_array_inx = 1;
                    return true;
                }
                else
                {
                    //failed checksum, need to clear this out anyway
                    rx_len = 0;
                    rx_array_inx = 1;
                    return false;
                }
            }
        }

        return false;
    }

    void KSPBoardSendData(uint8_t *address, uint8_t len)
    {
        uint8_t cs = len;
        ports[0].write(0xBE);
        ports[0].write(0xEF);
        ports[0].write(len);

        for (int i = 0; i < len; i++)
        {
            cs ^= *(address + i);
            ports[0].write(*(address + i));
        }

        ports[0].write(cs);
    }

    void Handshake()
    {
        handshakePkt.id = 0;
        handshakePkt.M1 = 3;
        handshakePkt.M2 = 1;
        handshakePkt.M3 = 4;

        KSPBoardSendData(details(handshakePkt));
    }

    int input()
    {
        int returnValue = -1;
        now = millis();

        if (KSPBoardReceiveData())
        {
            deadtimeOld = now;
            returnValue = id;
            switch (id)
            {
            case 0: //Handshake packet
                Handshake();
                break;
            case 1:
                // Indicators();
                break;
            }

            connected = true;
        }
        else
        { //if no message received for a while, go idle
            deadtime = now - deadtimeOld;
            if (deadtime > IDLETIMER)
            {
                deadtimeOld = now;
                connected = false;
            }
        }

        return returnValue;
    }

    void output()
    {
        now = millis();
        controlTime = now - controlTimeOld;
        if (controlTime > CONTROLREFRESH)
        {
            controlTimeOld = now;
            // TODO update controlPkt
            KSPBoardSendData(details(controlPkt));
        }
    }
};