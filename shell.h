#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_ARG_NUM 16
#define MAX_INPUT_LEN 255
#define MAX_CWD_LEN 255
#define MAX_CMD_NUM 127

typedef enum
{
    FALSE = 0,
    TRUE = 1
} bool_t;

typedef struct Command {
    char* args[MAX_ARG_NUM];
    int args_count;
    bool_t bg;
    int read_end_fd;
    int write_end_fd;
    bool_t last;
} command_t;

char ** tokenize(char input);
command_t * tokenize_init_single_cmd(char input[]);
void exectue(command_t *cmd_list[], const int list_size, char input_file[], char output_file[]);
void execute_single(command_t *cmd);

#endif /* ifndef SHELL_H */
