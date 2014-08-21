#include <hashmap.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#define SCMD_NAME_MAX 16
#define SCMD_SYNX_MAX 32
#define SCMD_DESC_MAX 128
#define SCMD_SYNOP_MAX 255

xHashMap_t shell_hmap;
FILE* _shell_cmd;

typedef struct {
    char* name;
    char* desc;
    char* syntax;
    char* synopsis;
    int(*fn)(int,char**);
} shell_cmd_t;


void shell_addcmd(char* key, char* name, char* desc, char* syntax, char* synop, int(*fn)(int,char**)) {
    shell_cmd_t s;
    s.fn = fn;
    s.name = name;
    s.desc = desc;
    s.syntax = syntax;
    s.synopsis = synop;
    xHashMapSet(&shell_hmap, key, &s);
}

int shell_exec(char* key, int argc, char **argv) {
    shell_cmd_t *cmd = xHashMapGet(&shell_hmap, key);
    if(cmd != NULL) 
        return cmd->fn(argc, argv);
    
    fprintf(_shell_cmd, "Unknown command: %s\r\n", key);
    return -1;
}

int __help(int argc, char** argv) {

    int cmd_count = 0, i;
    char* cmd_keys[20];
    
    fprintf(_shell_cmd, "DEBUG Command Details\r\n");
    for(i = 0; i < 40; i++) fputc('=', _shell_cmd);
    fputs("\r\n", _shell_cmd);

    if(argc > 1) {
        shell_cmd_t *cmd = xHashMapGet(&shell_hmap, argv[1]);
        if(cmd == NULL) {
            fprintf(_shell_cmd, "Unknown command: %s\r\n", argv[1]);
            return -1;
        }

        fprintf(_shell_cmd, "%s : %s\r\n\r\n", cmd->name, cmd->desc);
        fprintf(_shell_cmd, "SYNTAX: %s\r\n", cmd->syntax);
        fputs(cmd->synopsis, _shell_cmd);
        fputs("\r\n", _shell_cmd);

    } else {
        cmd_count = xHashMapKeys(&shell_hmap, cmd_keys);
        fputs("The following commands are available:\r\n", _shell_cmd);

        for(;cmd_count > 0; cmd_count--)
            fprintf(_shell_cmd, "  %s\r\n", cmd_keys[cmd_count-1]);

        fputs("\r\nGet more details on a command by running: help [CMD]\r\n", _shell_cmd);
    }
    return 0;
}

int __gps_dump(int argc, char** argv) {
    if(argc != 2) return -1;
    int count = atoi(argv[1]);
    FILE* gps = fopen("/dev/GPS", "r");

    if(gps == NULL) {
        fprintf(_shell_cmd, "Failed to open GPS Radio!\r\n");
        return -1;
    }

    fprintf(_shell_cmd, "Reading %i report(s) from the GPS Radio...\r\n", count);

    char c;

    // Find the beging of a report
    while(1) {
        c = fgetc(gps);
        if(c == '$') break;
    }

    // Dump reports
    while(count > 0) {
        c = fgetc(gps);
        if(c == '$') {
            count--;
            fputs("\r\n", _shell_cmd);
        } else if(c == '\r' || c == '\n') {
            continue;
        } else {
            fputc(c, _shell_cmd); fflush(_shell_cmd);
        }
    }

    fclose(gps);
    fputs("\r\n", _shell_cmd);
    return 0;
}

int __read(int argc, char** argv) {
    if(argc != 3) return -1;
    
    char* path = argv[1];
    int count = atoi(argv[2]);
    char c;

    FILE* fid = fopen(path, "r");
    if(fid == NULL) {
        fprintf(_shell_cmd, "Unable to open file: %s\r\n", path);
        return -1;
    }

    fprintf(_shell_cmd, "Dumping %i bytes from %s\r\n", count, path);
    for(;count > 0; count--) {
        c = fgetc(fid);
        fputc(c, _shell_cmd);
    }

    fputs("\r\n", _shell_cmd);

    fclose(fid);
    return 0;
}

int __mem_dump(int argc, char** argv) {
    if(argc != 4) return -1;

    int ptr = strtol(argv[2], NULL, 16);
    int count = atoi(argv[3]);
    int fmt = argv[1][0];

    void* _m = (void*)ptr;

    int movef = 0;

    fprintf(_shell_cmd, "Dumping memory at [0x%x]...\r\n", ptr);
    for(;count > 0; count--) {
        switch(fmt) {
            case 's':
                movef = strlen(_m);
                fprintf(_shell_cmd, "0x%x: %s\r\n", ptr, (char*)_m);
                break;
            case 'x':
                movef = sizeof(int);
                fprintf(_shell_cmd, "0x%x: 0x%x\r\n", ptr, (*(int*)_m));
                break;
            case 'i':
                movef = sizeof(int);
                fprintf(_shell_cmd, "0x%x: %i\r\n", ptr, (*(int*)_m));
                break;
            case 'b':
                movef = sizeof(char);
                fprintf(_shell_cmd, "0x%x: %x\r\n", ptr, (*(int*)_m) & 0x0FF);
                break;
        }

        ptr+=movef;
        _m = (void*)ptr;
    }

    return 0;
}

#include <stm32f10x.h>
#include <stm32f10x_i2c.h>
extern void i2c_BufferedRead(I2C_TypeDef* I2Cn, uint8_t slave, uint8_t *buffer, uint8_t addr, int count);
extern void i2c_WriteByte(I2C_TypeDef* I2Cn, uint8_t slave, uint8_t buffer, uint8_t addr);

//0110 1100
//0x6c

int __test(int argc, char** argv) {
    uint8_t p1, p2, x;
    uint16_t b;

    uint16_t volts;
    float perc;

    i2c_BufferedRead(I2C1, 0x6c, (uint8_t*)&b, 0x02, 2);
    i2c_BufferedRead(I2C1, 0x6c, &p1, 0x04, 1);
    i2c_BufferedRead(I2C1, 0x6c, &p2, 0x05, 1);

    i2c_BufferedRead(I2C1, 0x6c, &x, 0x0D, 1);

    volts = ((b>>8|b<<8)>>4);
    perc = p1 + (p2/256.0);
    x = (~x & 0x1F)+1;

    fprintf(_shell_cmd, "Bat0: %0.2f %% @ %0.2f mV\r\n", perc, volts*1.25);
    fprintf(_shell_cmd, "ALRT @ %u%%\r\n", x);

    return 0;
}

void shell_init(FILE* fid) {
    _shell_cmd = fid;
    xHashMapCreate(sizeof(shell_cmd_t), 20, &shell_hmap);

    shell_addcmd(
        "help",                                                         // Key
        "Help",                                                         // Name
        "Get details about a command",                                  // Desc
        "help [CMD]",                                                   // Synx
        " CMD - The command for which you are requesting help.\r\n",    // Synop
        &__help);

    shell_addcmd(
        "gps",
        "GPS Dump",
        "Dump data from the GPS Radio.",
        "gps COUNT",
        " COUNT - The number of reports to read from the gps buffer.\r\n",
        &__gps_dump);

    shell_addcmd(
        "mem",
        "Memory Dump",
        "Read data from memory",
        "mem FMT PTR COUNT",
        " FMT   - The format of the data to dump (s,x,i,b).\r\n"
        " PTR   - The address of the memory to dump in HEX.\r\n"
        " COUNT - The number of [FMT]s to read from memory.\r\n"
        "\r\n"
        "Format Details:\r\n"
        " s - Print as string\r\n"
        " x - Print as uint32 in HEX format.\r\n"
        " i - Print as integer in decimal format.\r\n"
        " b - Print as byte in HEX format.\r\n",
        &__mem_dump);

    shell_addcmd(
        "read",
        "Read File",
        "Read n bytes from a file",
        "read FILE COUNT",
        " FILE  - The path to the file.\r\n"
        " COUNT - The number of bytes to read.\r\n",
        &__read);

    shell_addcmd(
        "test",
        "Test",
        "Run a test subrutine",
        "test",
        "\r\n",
        &__test);
}

