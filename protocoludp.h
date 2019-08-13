#ifndef PROTOCOLUDP_H
#define PROTOCOLUDP_H
#include <stdint.h>
#pragma pack(push, 1)
    typedef struct _RF60x_UDP_VALUE_
    {
        uint16_t wMeasure;
        uint8_t bStatus;
    } RF60xUDPVALUE, *LPRF60xUDPVALUE;

    typedef struct _RF60x_UDP_PACKET_
    {
        RF60xUDPVALUE   rf60xValArray[168];
        uint16_t wDeviceSerial;
        uint16_t wDeviceBaseDistance;
        uint16_t wDeviceMeasureRange;
        uint8_t bPackCount;
        uint8_t bPacketControlSumm;
    } RF60xUDPPACKET, *LPRF60xUDPPACKET;
#pragma pack(pop)

#endif // PROTOCOLUDP_H
