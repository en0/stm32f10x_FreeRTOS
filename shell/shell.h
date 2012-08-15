#ifndef __SHELL_H__
#define __SHELL_H__

#define SHELL_CMD_BUFFER_LEN	128

#define SHELL_WELCOME_STRING	"Welcome\n"
#define SHELL_PROMPT_STRING	"# "

struct shell_command {
	char *name;
	void (*function )(int argc, char **argv);
};

void shell_init(void);

#endif /* __SHELL_H__ */
