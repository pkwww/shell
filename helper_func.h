#ifndef HELPER_FUNC
#define HELPER_FUNC

#include "shell.h"
void split(char **buff, char *str, const char *delim);
void clean_command_list(command_t *cmd_list[]);
void print_cmd(command_t *cmd);
void print_cmd_list(command_t *cmd_list[]);
void short_cwd(char cwd[]);
void parse_input_output(char cmd_string[], char input_file[], char output_file[]);

#endif
