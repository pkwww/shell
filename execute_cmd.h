#ifndef BUILDIN_CMD
#define BUILDIN_CMD

#include "shell.h"
#include <signal.h>
#include <errno.h>

void exec_exit(command_t *cmd);
void exec_cd(command_t *cmd);
void exec_fg(command_t *cmd, int (*pipes)[2], int num_of_pipes);
void exec_bg(command_t *cmd);
void child_sig_handler(int sig);
void close_unrelavent_fd(int read_fd, int write_fd, int (*pipes)[2], int list_size);

#endif
