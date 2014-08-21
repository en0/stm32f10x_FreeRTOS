#include <errno.h>
#undef errno
extern int errno;

#include <device.h>
#include <serial.h>
#include <hashmap.h>

xHashMap_t hmap;

// Holds File IDs for open devices.
#define MAX_FIDS 32
device_t* fids[MAX_FIDS];

/**                                                                         **
 ** This peice of code will manage all the devices added to the system.     **
 ** It is like a inode model but without a real filesystem we only need     **
 ** to manage items in the /dev directory.                                  **
 **                                                                         **/


/** Initialize the device map                       **
 ** Arguments:                                      **
 **  - The count of devices in the map              **
 **                                                 **
 ** Returns:                                        **
 **  - On success, returns 1;                       **
 **  - On error, 0 or lower and errno is set.       **/
int init_devicemap(size_t count) {

    //if(hmap.map == NULL) {
        //errno = EPERM;
        //return -1;
    //}

    xHashMapCreate(sizeof(device_t), (int)count, &hmap);
    return 1;
}


/** Add and initialize a new device.                                            **
 ** Arguments:                                                                  **
 **  - dtype : The device Type as device_type enumeration.                      **
 **  - cid : The channel ID associated with the type of device. ie: USARTa      **
 **  - conf : The configuration parameters specific for the device type.        **
 **                                                                             **
 ** Returns:                                                                    **
 **  - On success, returns 1;                                                   **
 **  - On error, 0 or lower and errno is set.                                   **/
int dev_mknode(const char* path, device_type dtype, uint32_t cid, void* conf) {

    device_t newDevice;

    newDevice.major = dtype;
    newDevice.minor = cid;
    newDevice.mode = 0;
    
    switch(dtype) {
        case serial_uart:
            serial_uart_init(&newDevice, conf);
            break;

        case serial_i2c:
            serial_i2c_init(&newDevice, conf);
            break;

        case serial_spi:
            serial_spi_init(&newDevice, conf);
            break;

        default:
            errno = ENXIO;
            return -1;
    }
    
    xHashMapSet(&hmap, path, &newDevice);
    return 1;
}


/** Open a device for IO                            **
 ** Arguments:                                      **
 **  - path  : The inode key to the device          **
 **  - flags : File Access Flags                    **
 **  - mode  : The open mode, R/W/RW                **
 **                                                 **
 ** Returns:                                        **
 **  - On success, returns the FID;                 **
 **  - On error, -1 and errno is set.               **/
int dev_open(const char* path, int flags, int mode) {
    device_t *node = xHashMapGet(&hmap, path);
    if(node == NULL) return -1;

    int i; //skip 0
    for(i = 1; i < MAX_FIDS; i++) {
        if(fids[i] == 0) {
            fids[i] = node;
            node->open(node, flags, mode);
            return i;
        }
    }

    errno = EMFILE;
    return -1;
}


/** Write a byte to an open device                  **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **  - v    : The value to write to the file        **
 **                                                 **
 ** Returns:                                        **
 **  - On success, The number of bytes writen (1)   **
 **  - On error, -1 and errno is set.               **/
int dev_writeByte(int fid, uint16_t v) {
    device_t *node = fids[fid];
    return node->write(node, v);
}


/** Read a byte from a open file.                   **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **  - *v   : A pointer to a recieve buffer.        **
 **                                                 **
 ** Returns:                                        **
 **  - On success, The number of bytes read. (1)    **
 **  - On error, -1 and errno is set.               **/
int dev_readByte(int fid, uint16_t *v) {
    device_t *node = (device_t*)fids[fid];
    return node->read(node, v);
}


/** Close an open device.                           **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **                                                 **
 ** Returns:                                        **
 **  - On success, returns 0;                       **
 **  - On error, -1 and errno is set.               **/
int dev_close(int fid) {
    device_t *node = fids[fid];
    if(node != NULL) {
        node->close(node);
        return 0;
    }
    return -1;
}
