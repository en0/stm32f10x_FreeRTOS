#include <FreeRTOS.h>
#include <uart.h>
#include <echo.h>
#include <task.h>

#include <stdio.h>
#include <device.h>
#include <string.h>

#define SCMD_MAX_ARGS 5

/** Shell Command Wrappers **/
extern void shell_addcmd(char* key, char* name, char* desc, int(*fn)(int,char**));
extern int shell_exec(char* key, int argc, char **argv);
extern void shell_init(FILE* fid);

int _prompt(FILE* fid, char* p, char* buffer, int len) {
    char c;
    int ret = 0;

    fprintf(fid, p); fflush(fid);

    for(;len > 0; len++) {
        c = fgetc(fid); fputc(c,fid); fflush(fid);
        if(c == '\n' || c == '\r') {
            fputs("\r\n", fid);
            break;
        }

        if(c == '\b') {
            *buffer-- = '\0';
            ret--;
        } else {
            *buffer++ = c;
            ret++;
        }
    }

    *buffer = '\0';
    return ret;
}


void _main(void* pvParams) {

    FILE *dbg = fopen("/dev/ttyUART3", "r+");
    shell_init(dbg);

    fputs("\r\nDebug Console v0.1\r\n"
          "------------------\r\n",dbg);

    char buffer[128];
    char *pch;
    char* argv[SCMD_MAX_ARGS];
    int i, ret;
    
    while(1) {
        _prompt(dbg, "[ DEBUG ] $ ", buffer, 128);
        pch = strtok(buffer, " ");
        for(i = 0; i < SCMD_MAX_ARGS && pch != NULL; i++) {
            argv[i] = pch;
            pch = strtok(NULL, " ");
        }
        ret = shell_exec(argv[0], i, argv);

        if(ret != 0)
            fprintf(dbg, "NOTICE: '%s' exited with status code %i\r\n", argv[0], ret);

    }
    fclose(dbg);
}


void echo_app() {

    xTaskCreate(
        _main, 
        (const signed char*)"ECHO", 
        configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL
    );

    
}
