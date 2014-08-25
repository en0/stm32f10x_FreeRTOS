#include <devlist.h>
#include <errno.h>
#undef errno
extern int errno;

#include <sys/stat.h>

extern void send_string(const char*);

/** 
    ********* About Device Drivers to be installed as nodes. ***********

    all device drivers will have a device_Install() function that will be called from wireup.c

    Only devices with an conf_ENABLE_PERIPH_XXXX defined in TrackerConfig will be enabled.

    Device drivers MUST check for existance of conf_ENABLE_PERIPH_XXXX. if it is not present then
    the file should result in an emtpy code file.

    Device drivers must support the Open, Close, Fstat, IsTTY, Read, Write, and lseek interfaces.

    Device drivers will get all their wireup details from TrackerConfig.h

    Device drivers will handle there own buffers for multi read stuff



    *********** About device drivers that will expose an API. ***********
    
    all device drivers will have a device_Install() function that will be called from wireup.c

    Only devices with an conf_ENABLE_PERIPH_XXXX defined in TrackerConfig will be enabled.

    Device drivers MUST check for existance of conf_ENABLE_PERIPH_XXXX. if it is not present then
    the file should result in an emtpy code file.

    Device drivers will get all their wireup details from TrackerConfig.h

 **/

int _fstat(int file, struct stat *st) {
    send_string("\n/** [ _fstat ] **/\n");
    return devlist_fStat(file, st);

    /** 
        Call: dev_fstat(fid, st)
        
        dev_fstat: (Device Map Level)
         - lookup the node
         - if not exists, ERROR: EBADF
         - Set the following fields
           st_dev = map id number
           st_ino, st_nlink, st_uid, st_gid = 0

         - Call: node->fstat(fid, st)
         
        node->fstat: (Driver Level)
         - Set the following fields
           st_mode = devices mode (File type)
           st_size = if applicable, else zero
           st_blksize = if applicable, else zero
           st_blocks = if applicable, else zero

        return 0 if success, else return -1
     **/
}


int _open(const char *name, int flags, int mode) {
    send_string("\n/** [ _open ] **/\n");
    return devlist_Open(name, flags, mode);

    /**
        Call: dev_open(name, flags, mode;
        
        dev_open: (Device Map Level)
         - Check flags:
         - If any type of create request: EINVAL
         - If any type of O_DIRECT, ERROR: EINVAL
         - If any type of O_DIRECTORY, ERROR: EINVAL
         - If any type of O_TMPFILE, ERROR: EINVAL
         - If any type of O_PATH, ERROR: EINVAL
         - If any type of O_NONBLOCK or O_NDELAY: EINVAL
         - If any type of O_NOFOLLOW or O_NDELAY: EINVAL
         - If any type of O_NOCITY or O_NDELAY: EINVAL
         - Find the node
         - If not found, ERROR: EBADF
         - Check the requested access mode agaist _fstate st_mode.
         - If write request with no write support: EROFS
         - If Read request with no read support: EACCESS
         - Get the next FID
         - Store access mode with fid (open_node_t->fid, open_node_t->mode, open_node->node = node)
         - Call: node->open(fid, flags, mode)

        node->open: (Driver Level)
         - Check capabilities against request
         - Check O_TRUNC. If not supported, ERROR: EINVAL
         - Check O_APPEND. if not supported, error: EINVAL

         - Deal with driver specific things
           - If buffers are needed, create them
           - If blocking is needed, set it up
           - Create file pointers if needed
           - Store in interal structure using the fid as a key

        If any errors, return -1
        else return fid

     **/
}


int _write(int file, char *ptr, int len) {
    send_string("\n/** [ _write ] **/\n");
    return devlist_Write(file, ptr, len);

    /** 
        Call: dev_write(fid, ptr, len)
        
        dev_write: (Device Map Level)
         - lookup the open_node_t
         - if not exists, ERROR: EBADF
         - if not RDWR or WRONLY, ERROR: EACCES
         - Call: open_node->node->write(fid, ptr, len)
         
        node->write: (Driver Level)
        - Lookup the driver specific things for the FID.
        - Do what is needed to write to ptr
        - return count of chars read.

        return COUNT OF CHARS if success, else return -1;
     **/
}

int _close(int file) {
    send_string("\n/** [ _close ] **/\n");
    return devlist_Close(file);

    /** 
        Call: dev_close(fid, ptr, len)
        
        dev_close: (Device Map Level)
         - lookup the open_node_t
         - if not exists, ERROR: EBADF
         - Call: open_node->node->read(fid, ptr, len)
         
        node->close: (Driver Level)
        - Lookup the driver specific things for the FID.
        - Do what is needed to close the IO.
        - Clean up driver specific things for this FID.

        return 0 if success, else return -1;
     **/
}


int _isatty(int file) {
    send_string("\n/** [ _isatty ] **/\n");
    return devlist_IsTTY(file);

    /** 
        Call: dev_istty(fid)
        
        dev_istty: (Device Map Level)
         - lookup the open_node_t
         - if not exists, ERROR: EBADF
         - if not RDWR or RDONLY, ERROR: EACCES
         - Call: open_node->node->istty(fid)
         
        node->istty: (Driver Level)
         - return True or False as needed

        return 1 or 0 if success, else return -1;
     **/
}

int _read(int file, char* ptr, int len) {
    send_string("\n/** [ _read ] **/\n");
    return devlist_Read(file, ptr, len);

    /** 
        Call: dev_read(fid, ptr, len)
        
        dev_read: (Device Map Level)
         - lookup the open_node_t
         - if not exists, ERROR: EBADF
         - if not RDWR or RDONLY, ERROR: EACCES
         - Call: open_node->node->read(fid, ptr, len)
         
        node->read: (Driver Level)
        - Lookup the driver specific things for the FID.
        - Do what is needed to read to ptr
        - return count of chars read.

        return COUNT OF CHARS if success, else return -1;
     **/
}

int _lseek(int file, int ptr, int dir) {
    send_string("\n/** [ _lseek ] **/\n");
    return devlist_lSeek(file, ptr, dir);

    /** 
        Call: dev_lseek(fid, ptr, dir)
        
        dev_lseek: (Device Map Level)
         - lookup the open_node_t
         - if not exists, ERROR: EBADF
         - if not RDWR or RDONLY, ERROR: EACCES
         - Call: open_node->node->lseek(fid, ptr, len)
         
        node->lseek: (Driver Level)
        - if cannot be seeked, ERROR: ESPIPE
        - Lookup the driver specific things for the FID.
        - Do what is needed to set the file ptr
        - return the offset from the beginning of the file

        return OFFSET FROM THE BEGINNING OF THE FILE if success, else return -1;
     **/
}

