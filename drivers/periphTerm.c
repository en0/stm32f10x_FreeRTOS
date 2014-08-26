/*
    ********* About Device Drivers to be installed as nodes. ***********

    all device drivers will have a device_Install() function that will be called from wireup.c

    Only devices with an conf_ENABLE_PERIPH_XXXX defined in TrackerConfig will be enabled.

    Device drivers MUST check for existance of conf_ENABLE_PERIPH_XXXX. if it is not present then
    the file should result in an emtpy code file.

    Device drivers must support the Open, Close, Fstat, IsTTY, Read, Write, and lseek interfaces.

    Device drivers will get all their wireup details from TrackerConfig.h

    Device drivers will handle there own buffers for multi read stuff
*/

#include <errno.h>
#undef errno
extern int errno;

#include <periphTerm.h>
#include <devlist.h>

#ifdef conf_ENABLE_PERIPH_TERM_USART

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <gpio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    xQueueHandle* rx;
    bool wloc;
} _term_fildes_t;

_term_fildes_t *_term_fildes[conf_MAX_OPEN_FILE_IDS] = {0};

// Remove these
#include <stdio.h>
extern void send_string(const char*);
extern void send_memory(void* ptr, size_t len);
extern void send_integer(const char* fmt, uint32_t i);

/******
    This is a temp implementation. We need to setup locking but first
    i want to make sure the driver structure works before burning a
    bunch of time on FID locks.
 ******/


/** Initiate the hardware **/

void _term_init_hw() {
    // Setup Clocks
    conf_TERM_GPIO_RCC_PeriphClockCmd(conf_TERM_GPIO_RCC_Periph_GPIOx, ENABLE);
    conf_TERM_USART_RCC_PeriphClockCmd(conf_TERM_USART_RCC_Periph_USARTx, ENABLE);

    // Setup GPIO Pin Modes
    gpio_config(conf_TERM_GPIO, conf_TERM_GPIO_RX, GPIO_Mode_IN_FLOATING, 0);
    gpio_config( conf_TERM_GPIO, conf_TERM_GPIO_TX, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);

    // Configure the UART 
    USART_InitTypeDef conf;
    conf.USART_BaudRate = conf_TERM_USART_BaudRate;
    conf.USART_WordLength = conf_TERM_USART_WordLength;
    conf.USART_StopBits = conf_TERM_USART_StopBits;
    conf.USART_Parity = conf_TERM_USART_Parity;
    conf.USART_HardwareFlowControl = conf_TERM_USART_HardwareFlowControl;
    conf.USART_Mode = conf_TERM_USART_Mode;

    // Enable interrupts on this UART
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = conf_TERM_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    // Enable the uart
	USART_ITConfig(conf_TERM_USART, USART_IT_RXNE, ENABLE);
    USART_Init(conf_TERM_USART, &conf);
    USART_Cmd(conf_TERM_USART, ENABLE);
}

static bool HAS_WLOC = false;

/** REGION: Driver Interface **/

int _term_open(int fid, int flags, int mode) { 

    #define CK_FLG(f) ((flags & f) == f)

    // Do we want wloc on this request?
    bool wants_wloc = (CK_FLG(O_WRONLY) || CK_FLG(O_RDWR) ? true : false);

    if(wants_wloc) {

        taskENTER_CRITICAL();

        // Do we already have a wlock?
        if(HAS_WLOC) {

            taskEXIT_CRITICAL();

            errno = EAGAIN;
            return -1;
        } else {
            HAS_WLOC = true;
        }

        taskEXIT_CRITICAL();
    }


    if(_term_fildes[fid] != NULL) { errno = EMFILE; return -1; }

    _term_fildes[fid] = malloc(sizeof(_term_fildes_t));

    if(CK_FLG(O_RDONLY) || CK_FLG(O_RDWR)) {
        // Allocate recieve queue
        _term_fildes[fid]->rx = xQueueCreate(conf_TERM_USART_QUEUESIZE, sizeof(uint8_t));
    } else {
        _term_fildes[fid]->rx = NULL;
    }

    _term_fildes[fid]->wloc = wants_wloc;
    
    return fid; 
}

int _term_close(int fid) { 

    _term_fildes_t* fildes = _term_fildes[fid];

    taskENTER_CRITICAL();
    taskDISABLE_INTERRUPTS();

    // Clear the FID entry
    _term_fildes[fid] = NULL;

    // Free queue
    if(fildes->rx != NULL) vQueueDelete(fildes->rx);

    // Free wloc
    if(fildes->wloc) HAS_WLOC = false;

    taskDISABLE_INTERRUPTS();
    taskEXIT_CRITICAL();

    free(fildes);
    return 0; 
}

int _term_fstat(int fid, struct stat *st) { 
    st->st_mode = S_IFCHR;
    return 0; 
}

int _term_istty(int fid) { 
    return 1; 

}

int _term_read(int fid, char *ptr, int len) { 

    int i = 0;
    char v;

    _term_fildes_t* fildes = _term_fildes[fid];

    while(len > 0) {

        if(xQueueReceive(fildes->rx, &v, (portTickType)10)) {
            *ptr++ = v;
        } else if(i == 0) {
            taskYIELD();
            continue;
        } else
            break;

        len--;
        i++;
    }

    return i; 
}

int _term_writebyte(uint8_t v) {
    uint16_t value = v;

    taskENTER_CRITICAL();

    USART_SendData(conf_TERM_USART, value);
    while(USART_GetFlagStatus(conf_TERM_USART, USART_FLAG_TXE) == RESET);

    taskEXIT_CRITICAL();

    return 1;
}

int _term_write(int fid, char *ptr, int len) { 
    int i;
    for(i = 0;len > 0; len--)
        i += _term_writebyte(*ptr++);
    return i;
}

int _term_lseek(int fid, int ptr, int wence) { 

    return 0; 
}

/** END REGION: Driver Interface **/


/** REGION: Interrupt Hander **/

void conf_TERM_USART_IRQHandler(void) {

    int fid;
    uint8_t c;
    portBASE_TYPE xHigherPriorityTaskWoken = 0;

    if(USART_GetITStatus(conf_TERM_USART, USART_IT_RXNE) != RESET) {
        c = (uint8_t)USART_ReceiveData(conf_TERM_USART);

        // Send input to each reading FID
        for(fid = 0; fid < conf_MAX_OPEN_FILE_IDS; fid++) {
            if(_term_fildes[fid] != NULL && _term_fildes[fid]->rx != NULL)  {
                xQueueSendToBackFromISR(_term_fildes[fid]->rx, &c, &xHigherPriorityTaskWoken);
            }
        }
    }
}

/** END REGION: Interrupt Handler **/


/** Install the driver **/

void term_Install() {

    _term_init_hw();

    node_t n;
    n.path = conf_DEVLIST_PATH_TERM_USART;
    n.mode = O_RDWR;
    n.open = &_term_open;
    n.close = &_term_close;
    n.fstat = &_term_fstat;
    n.istty = &_term_istty;
    n.read = &_term_read;
    n.write = &_term_write;
    n.lseek = &_term_lseek;

    devlist_MkNode(&n);

    // Install stdio 
    stderr = stdout = fopen("/dev/tty0", "w");
    stdin = fopen("/dev/tty0", "r");
}

#endif /* conf_ENABLE_PERIPH_TERM_USART */


