#ifndef __DEVLIST_H
#define __DEVLIST_H 1

#include <sys/stat.h>

/** Describes a node in the device Linked list              **
 ** Each node is added by a call to devicelist_mknode       **
 ** The function will allocate memory for this structure    **
 ** and append it to the linked list.                       **
 ** There is very little need to have a device cleanup      **/

typedef struct {
    int mapid;
    const char *path;
    int mode;
    int(*open)(int fid, int flags, int mode);
    int(*close)(int fid);
    int(*fstat)(int fid, struct stat *st);
    int(*istty)(int fid);
    int(*read)(int fid, char *ptr, int len);
    int(*write)(int fid, char *ptr, int len);
    int(*lseek)(int fid, int ptr, int wence);
} node_t;

/** This function will create a new node in the device list         **
 ** Each item in the device list is stored in a hash map and        **
 ** it's driver functions will be called when access is requested.  **
 **                                                                 **
 ** Arguments:                                                      **
 **  key     - The path to the node                                 **
 **  sa_mode - The access mode this node can be opened with         **
 **  open    - The driver function to open                          **
 **  close   - The driver function to close                         **
 **  fstat   - The driver function to fstat                         **
 **  istty   - The driver function to istty                         **
 **  read    - The driver function to read                          **
 **  write   - The driver function to write                         **
 **  lseek   - The driver function to lseek                         ** 
 **                                                                 **
 ** Returns:                                                        **
 **  On Success: Returns the device mapid                           **
 **  On Error: Returns -1                                           **/

int devlist_MkNode(node_t *n);
/*int devlist_MkNode(
    const char *key,
    int supported_accessmode,
    int(*open)(int fid, int flags, int mode),
    int(*close)(int fid),
    int(*fstat)(int fid, struct stat *st),
    int(*istty)(int fid),
    int(*read)(int fid, char *ptr, int len),
    int(*write)(int fid, char *ptr, int len),
    int(*lseek)(int fid, int ptr, int wence)
);*/


// Other functions defined in dev
int devlist_Open (const char *name, int flags, int mode);
int devlist_Close (int fid);
int devlist_fStat (int fid, struct stat *st);
int devlist_IsTTY (int fid);
int devlist_Read (int fid, char *ptr, int len);
int devlist_Write (int fid, char *ptr, int len);
int devlist_lSeek (int fid, int ptr, int wence);

#endif /** __DEVLIST_H **/

