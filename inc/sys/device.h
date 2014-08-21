#ifndef __DEVICE_H
#define __DEVICE_H

#include <stdint.h>
#include <stdlib.h>

#define SERIAL_QUEUE_SIZE 32


/** Wireup user configured hardware map **/
extern void init_wireup();


/** Generic Device Structure **/

typedef enum {
    serial_uart = 1,
    serial_i2c = 2,
    serial_spi = 3,
} device_type ;

typedef enum {
    dev_read = 1,
    dev_write = 2,
} device_mode_t;

struct _device_t {
    device_type major;
    uint32_t minor;
    device_mode_t mode;
    int(*open)(struct _device_t*, int flags);
    int(*close)(struct _device_t*);
    int(*read)(struct _device_t*, uint16_t *v);
    int(*write)(struct _device_t*, uint16_t v);
};

typedef struct _device_t device_t;


/** Initialize the device map                   **
 ** Arguments:                                  **
 **  - The count of devices in the map          **
 **                                             **
 ** Returns:                                    **
 **  - On success, returns 1;                   **
 **  - On error, 0 and errno is set.            **/
int init_devicemap(size_t count);


/** Add and initialize a new device.                                        **
 ** Arguments:                                                              **
 **  - dtype : The device Type as device_type enumeration.                  **
 **  - cid : The channel ID associated with the type of device. ie: USARTa  **
 **  - conf : The configuration parameters specific for the device type.    **
 **                                                                         **
 ** Returns:                                                                **
 **  - On success, returns 1;                                               **
 **  - On error, 0 and errno is set.                                        **/
int dev_mknode(const char* path, device_type dtype, uint32_t cid, void* conf);


/** Open a device for IO                                **
 ** Arguments:                                          **
 **  - path  : The inode key to the device              **
 **  - flags : File Access Flags (RDONLY,WRONLY,RDRW)   **
 **                                                     **
 ** Returns:                                            **
 **  - On success, returns the FID;                     **
 **  - On error, -1 and errno is set.                   **/
int dev_open(const char* path, int flags);


/** Write a byte to an open device                  **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **  - v    : The value to write to the file        **
 **                                                 **
 ** Returns:                                        **
 **  - On success, The number of bytes writen (1)   **
 **  - On error, -1 and errno is set.               **/
int dev_writeByte(int fid, uint16_t v);


/** Read a byte from a open file.                   **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **  - *v   : A pointer to a recieve buffer.        **
 **                                                 **
 ** Returns:                                        **
 **  - On success, The number of bytes read. (1)    **
 **  - On error, -1 and errno is set.               **/
int dev_readByte(int fid, uint16_t *v);


/** Close an open device.                           **
 ** Arguments:                                      **
 **  - fid  : The File id of the open file.         **
 **                                                 **
 ** Returns:                                        **
 **  - On success, returns 0;                       **
 **  - On error, -1 and errno is set.               **/
int dev_close(int fid);


#endif /** __DEVICE_H **/

