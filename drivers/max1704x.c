#include <max1704x.h>
#include <stdio.h>
#include <i2c.h>

int __max1704x_WriteStringToMemoryStream(uint32_t minor, char* string) {
    return 0;
}


int max1704x_VoltageRefresh(uint32_t minor, int flags) {
    char buf[16] = {0};
    uint16_t b, volts;

    // Read the Battery Voltage and convert it.
    i2c_BufferedRead(0, 0x6c, (uint8_t*)&b, 0x02, 2);
    volts = ((b>>8|b<<8)>>4);

    // Format status file.
    sprintf(buf, "Bat0: %0.2f mV", volts*1.25);

    // Write to status file
    //MemoryStream_Truncate(minor);
    return __max1704x_WriteStringToMemoryStream(minor, buf);
}

int max1704x_PercRefresh(uint32_t minor, int flags) {
    return 0;
}

int max1704x_AlertRefresh(uint32_t minor, int flags) {
    return 0;
}

int max1704x_AlertCommit(uint32_t minor) {
    return 0;
}

