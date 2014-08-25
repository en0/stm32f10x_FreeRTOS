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

#include <periphTerm.h>
#include <devlist.h>

#ifdef conf_ENABLE_PERIPH_TERM_USART

#include <gpio.h>
#include <fcntl.h>
#include <sys/stat.h>


// Remove these
#include <stdio.h>
extern void send_string(const char*);
extern void send_memory(void* ptr, size_t len);

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


/** REGION: Driver Interface **/

int _term_open(int fid, int flags, int mode) { 
    //send_string("/** [ _term_open: HIT, yeah! ] **/\r\n"); 
    return fid; 
}

int _term_close(int fid) { 

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

    return -1; 
}

int _term_writebyte(uint8_t v) {
    uint16_t value = v;
    USART_SendData(conf_TERM_USART, value);
    while(USART_GetFlagStatus(conf_TERM_USART, USART_FLAG_TXE) == RESET);
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


