#ifndef STREAM_H
#define STREAM_H

#include <LUFA/Drivers/USB/USB.h>
#include "millis.h"

class Stream
{
    USB_ClassInfo_CDC_Device_t* CDCInterfaceInfo;

public:
    Stream(USB_ClassInfo_CDC_Device_t* VirtualSerial) : CDCInterfaceInfo(VirtualSerial) {}

    int available()
    {
        return CDC_Device_BytesReceived(CDCInterfaceInfo);
    }

    int read()
    {
        return CDC_Device_ReceiveByte(CDCInterfaceInfo);
    }

    size_t readBytes(char *buffer, size_t length)
    {
        size_t i;
        int16_t byte;

        for (i = 0; i < length; i++)
        {
            byte = read();

            if (byte >= 0)
            {
                buffer[i] = (char)byte;
                print(buffer[i]);
            }
            else
                break;
        }

        return i;
    }

    size_t print(const char msg[])
    {
        CDC_Device_SendString(CDCInterfaceInfo, msg);
        CDC_Device_Flush(CDCInterfaceInfo);
        return 0; // TODO: Count number bytes actually sent
    }

    size_t print(const char msg)
    {
        CDC_Device_SendByte(CDCInterfaceInfo, (unsigned char)msg);
        CDC_Device_Flush(CDCInterfaceInfo);
        return 0; // TODO: Count number bytes actually sent
    }

    size_t print(const unsigned char msg)
    {
        CDC_Device_SendByte(CDCInterfaceInfo, msg);
        CDC_Device_Flush(CDCInterfaceInfo);
        return 0; // TODO: Count number bytes actually sent
    }

    size_t println()
    {
        CDC_Device_SendString(CDCInterfaceInfo, "\r\n");
        CDC_Device_Flush(CDCInterfaceInfo);
        return 0; // TODO: Count number bytes actually sent
    }
};

#endif // STREAM_H
