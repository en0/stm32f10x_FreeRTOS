#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#undef errno
extern int errno;

#include <TrackerConfig.h>
#include <devlist.h>
#include <llist.h>

#include <stdio.h>
extern void send_string(const char* str);

/** Entry in the open file nodes list.                      **
 ** Each time a file is opened, the node is discovered      **
 ** The first available FID will be located in the          **
 ** open_node_list. The driver's open function is called.   **
 ** on return, memory will be allocated for a open_node_t   **
 ** and it will be assinged in the open file array          **
 ** When a file is closed, the open_node_id will be located **
 ** and the driver's close function will be called.         **
 ** on return, the open_node memory will be freed and the   **
 ** FID offset in the open_node_list will be NULLed.        **/

typedef struct {
    int fid;
    int mode;
    node_t *node;
} open_node_t;


/** This will point to linked list for the device map.          **/

int devlist_NodeCount = 0;
xLList_t *devlist_llist = (xLList_t *)NULL;


/** The open_nodes list is a fixed size. Each open node will    **
 ** have a refrence in this list. Empty entries will be NULL    **/

open_node_t *devlist_open_nodes[conf_MAX_OPEN_FILE_IDS] = { 0 };


/** Find the first available FID **/

int _devlist_nextFid() {
    int i;
    for(i = 0; i < conf_MAX_OPEN_FILE_IDS; i++)
        if(devlist_open_nodes[i] == NULL) return i;
    return -1;
}


/** Find a specific node by it's name **/

node_t* _devlist_findNode(const char* name) {

    node_t *ret;
    xLList_Node_t *node;

    //char buffer[100];
    //sprintf(buffer, "/** devlist_findNode: Looking for node: %s\n", name);
    //send_string(buffer);

    for(node = devlist_llist->root; node != NULL; node = node->next) {
        ret = (node_t*)node->data;

    //sprintf(buffer, "/** devlist_findNode: testing \"%s\" == \"%s\"\n", name, ret->path);
    //send_string(buffer);

        if(strcmp(ret->path, name) == 0) {

    //send_string("/** devlist_findNode: Found it!\n");

            return (node_t*)node->data;
            
        }
    //send_string("/** devlist_findNode: Trying the next one.\n");
    }

    //send_string("/** devlist_findNode: These aren't the nodes your looking for.\n");

    return NULL;
}


/** Safely find an open node by fid **/

open_node_t* _devlist_findOpenNode(int fid) {
    if(fid < 0 || fid >= conf_MAX_OPEN_FILE_IDS)
        return NULL;
    return devlist_open_nodes[fid];
}


/** Add a new node to the devicelist using xLList linked list.  **
 ** Fill a node_t struct and add it to the devlist_llist. The   **
 ** xLList code will allocate, copy, and link it for latter use **/

int devlist_MkNode(node_t *n) {

    // Create the device list
    if(devlist_llist == (xLList_t *)NULL) 
        devlist_llist = xLList_Create(sizeof(node_t));

    // Add the new node to the list
    if(xLList_Append(devlist_llist, n) != NULL) return 0;

    // Something whent wrong
    return -1;
}


/** Locate a new FID entry, validate request, call driver OPEN, and store open_node_t details in fid entry **/

int devlist_Open (const char *name, int flags, int mode) { 

    //send_string("/** devlist_open: Attempting to open file.\n");
    // Find first FID available
    int fid = _devlist_nextFid();
    if(fid < 0) { errno = ENFILE; return -1; }
    //send_string("/** devlist_open: Found a free FID.\n");

    //send_string("/** devlist_open: Validating request.\n");
    // Verify we support the requested mode
    #define ASSERT_NOT_FLAGGED(f) if((flags & f) == f) { errno=EINVAL; return -1; }
    //ASSERT_NOT_FLAGGED(O_CREAT);
    ASSERT_NOT_FLAGGED(O_NONBLOCK);
    ASSERT_NOT_FLAGGED(O_NOCTTY);
    //ASSERT_NOT_FLAGGED(O_DIRECT);
    //ASSERT_NOT_FLAGGED(O_DIRECTORY);
    //ASSERT_NOT_FLAGGED(O_TMPFILE);
    //ASSERT_NOT_FLAGGED(O_PATH);
    //ASSERT_NOT_FLAGGED(O_NDELAY);
    //ASSERT_NOT_FLAGGED(O_NOFOLLOW);
    //ASSERT_NOT_FLAGGED(O_NDELAY);
    //ASSERT_NOT_FLAGGED(O_NDELAY);
    //send_string("/** devlist_open: Request valid.\n");

    //send_string("/** devlist_open: Finding the requested node.\n");
    // Find the node entry
    node_t* node =  _devlist_findNode(name);
    if(node == NULL) { errno = EBADF; return -1; }
    //send_string("/** devlist_open: Found it!\n");

    // Check the requested access mode agaist _fstate st_mode.
    int accmode = flags & O_ACCMODE;
    int n_accmode = node->mode & O_ACCMODE;

    //send_string("/** devlist_open: Checking access mode!\n");
    // If write request with no write support: EROFS
    if((accmode == O_RDONLY || accmode == O_RDWR) && (n_accmode != O_RDONLY && n_accmode != O_RDWR)) {
        errno = EROFS; 
        return -1;
    }
    //send_string("/** devlist_open: Read looks good!\n");

    // If Read request with no read support: EACCESS
    if((accmode == O_WRONLY || accmode == O_RDWR) && (n_accmode != O_WRONLY && n_accmode != O_RDWR)) {
        errno = EACCES;
        return -1;
    }
    //send_string("/** devlist_open: Write looks good!\n");

    //send_string("/** devlist_open: Attempting to call driver's OPEN function.\n");
    // Call Driver open implementation. If it returns anything other then the fid, an error occurned
    // It is assumed that errno is set by the driver. 
    // ** IMPORTATNT: The driver must check it's own write lock state. Use a mutex to ensure thread safty.
    if(node->open(fid, flags, mode) == fid) {
        //send_string("/** devlist_open: File is open, save details and FID.\n");
        // Add the new FID into the open file list.
        devlist_open_nodes[fid] = (open_node_t*)malloc(sizeof(open_node_t));
        devlist_open_nodes[fid]->fid = fid;
        devlist_open_nodes[fid]->mode = flags;
        devlist_open_nodes[fid]->node = node;
        //send_string("/** devlist_open: File is open: Returning!\n");
    } else {
        //send_string("/** devlist_open: DRIVER FAILED!!\n");
        fid = -1;
    }

    return fid;
}


/** Close a open file and clear the memory used by the open file tracking.  **
 ** Also free up the FID used by this open file                             **/

int devlist_Close (int fid) {
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    // Call driver close function
    int ret = open_node->node->close(fid);

    // Free open_node structure
    free(open_node);

    // clear the FID entry
    devlist_open_nodes[fid] = (open_node_t*)NULL;

    return ret;
}


/** Set the dev id and call the driver fstat function **/

int devlist_fStat (int fid, struct stat *st) { 
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    // Set the few things we know.
    st->st_dev = open_node->node->mapid;
    st->st_ino = st->st_nlink = st->st_uid = st->st_gid = 0;
    
    return open_node->node->fstat(fid, st);
}


/** Call driver implementation of istty **/

int devlist_IsTTY (int fid) { 
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    return open_node->node->istty(fid);
}


/** Verify the FID has access to read and call driver read function **/

int devlist_Read (int fid, char *ptr, int len) {
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    // Verify open file has access to read 
    int accmode = open_node->mode & O_ACCMODE;
    if(accmode != O_RDONLY && accmode != O_RDWR) {
        errno = EACCES;
        return -1;
    }

    return open_node->node->read(fid, ptr, len);
}


/** Verify FID has access to write and call driver write function **/

int devlist_Write (int fid, char *ptr, int len) {
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    // Verify open file has access to write 
    int accmode = open_node->mode & O_ACCMODE;
    if(accmode != O_WRONLY && accmode != O_RDWR) {
        errno = EACCES;
        return -1;
    }

    return open_node->node->write(fid, ptr, len);
}


/** Verify FID has access to read or write and call driver write function **/

int devlist_lSeek (int fid, int ptr, int wence) {
    // Find the node entry
    open_node_t* open_node =  _devlist_findOpenNode(fid);

    // If bad refrence, return error
    if(open_node == NULL) { errno = EBADF; return -1; }

    // Verify open file has access to write 
    int accmode = open_node->mode & O_ACCMODE;
    if(accmode != O_WRONLY && accmode != O_RDWR && accmode != O_RDONLY) {
        errno = EACCES;
        return -1;
    }

    return open_node->node->lseek(fid, ptr, wence);
}

