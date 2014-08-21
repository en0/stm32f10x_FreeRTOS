#include <errno.h>
#undef errno
extern int errno;

#include <FreeRTOS.h>
#include <task.h>
#include <device.h>
#include <sys/stat.h>

#include <stdio.h>
#include <fcntl.h>


extern void send_string(const char*);

/** Get stats of a open file.                                           **
 ** Arguments:                                                          **
 **  - file  - The FID of the file to stat                              **
 **  - *st   - The stat structure used to return details of the file    **/
int _fstat(int file, struct stat *st) {
    // Currently all files are CHAR devices.
    st->st_mode = S_IFCHR;
    return 0;
}


/** Open a file or device for read/write operation                      **
 ** Arguments:                                                          **
 **  - name  - The path to the file.                                    **
 **  - flags - The flags, O_APPEND or O_CREATE.                         **
 **  - mode  - the umask used for creating a file. DOES NOT APPLY YET   **/
int _open(const char *name, int flags, int mode) {
    // Currently, no support for MODE operations
    return dev_open(name, flags);
}


/** Write data to an open file.                                         **
 ** Arguments:                                                          **
 **  - file - The FID of the open file.                                 **
 **  - *ptr - A pointer to the data to be writen                        **/
int _write(int file, char *ptr, int len) {
    //send_string("-k, i got an write request\n\n");
    int i;
    for(i = 0; i < len; i++) {
        dev_writeByte(file, ptr[i]);
    }
    return i;
}

/** Close an open file.                                                 **
 ** Arguments:                                                          **
 **  - file - The FID of the open file                                  **/
int _close(int file) {
    return dev_close(file);
}


/** Check if an open file is ATTY or not                                **
 ** Arguments:                                                          **
 **  - file - The FID of the open file.                                 **/
int _isatty(int file) {
    //Currently all devices are atty devices.
    return 1;
}

/** Read data from a open file.                                         **
 **                                                                     **
 ** Read promies to return at least 1 byte or error.                    **
 ** So this function will block untill atleast one                      **
 ** byte becomes available.                                             **
 **                                                                     **
 ** Arguments:                                                          **
 **  - file  : The FID of the open file.                                **
 **  - *ptr  : The buffer to store the new data.                        **
 **  - len   : The max amount of bytes to read.                         **
 **                                                                     **
 ** Returns:                                                            **
 **  If successfull, The number of bytes read.                          **
 **  else, -1 and errno is set.                                         **/
int _read(int file, char* ptr, int len) {
    //Promise 1 byte then return what it can.
    // If needed, block for the 1 byte.
    uint16_t v; 
    int r, ret;
    for(ret = 0;ret < len;) {
        r = dev_readByte(file, &v);

        // Error : errno is set by dev_readByte
        if(r < 0) 
            return -1;

        // Nothing left to read.
        else if(r == 0 && ret > 0) 
            break;

        // Block untill atleast 1 byte is read.
        else if(r == 0) {
            //vTaskDelay(10);
            continue;
        }

        *ptr++ = (char)v;
        ret++;
    }
    return ret;
}

int _lseek(int file, int ptr, int dir) {
    send_string("Ok, i got an lseek request\n\n");
    return 0;
}

