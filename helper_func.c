#include "helper_func.h"

void split(char **buff, char *str, const char *delim)
{
    char *s = strtok(str, delim);
    while(s != NULL) {
        *buff = s;
        ++buff;
        s = strtok(NULL, delim);
    }
}

void clean_command_list(command_t *cmd_list[])
{
    int i;
    for (i = 0; i < MAX_CMD_NUM && cmd_list[i] != NULL; ++i) {
        int j;
        for (j = 0; j < MAX_ARG_NUM; ++j) {
            if (cmd_list[i]->args[j] != NULL) {
                free(cmd_list[i]->args[j]);
            }
        }
        free(cmd_list[i]);
        cmd_list[i] = NULL;
    }
}

void print_cmd(command_t *cmd)
{
    printf("command name: %s\n", cmd->args[0]);
    int i = 0;
    while (cmd->args[i]) {
        printf("arg %d: %s\n", i, cmd->args[i]);
        i++;
    }
    if (cmd->bg == TRUE) {
        printf("bg: true\n");
    } else {
        printf("bg: false\n");
    }
}

void print_cmd_list(command_t *cmd_list[])
{
    int i;
    for (i = 0; i < MAX_CMD_NUM && cmd_list[i] != NULL; ++i) {
        printf("%d-th cmd\n", i);
        print_cmd(cmd_list[i]);
        printf("\n");
    }
}

/**
 * To extract the last part of the absolute director
 * i.e. if current dir is /test/sth/good , it will assign
 * "good" to cwd. If the dir is / (the root dir), it will
 * assign "/" to cwd. */
void short_cwd(char cwd[])
{
    char long_cwd[MAX_CWD_LEN];
    getcwd(long_cwd, MAX_CWD_LEN);

    char *dir_seq[MAX_CWD_LEN];
    int i;
    for (i = 0; i < MAX_CWD_LEN; ++i) {
        dir_seq[i] = NULL;
    }
    split(dir_seq, long_cwd, "/");

    // when there is nothing after split, it means that it is the root dir,
    // so give it the symbol '/'
    if (dir_seq[0] == NULL) {
        cwd[0] = '/';
        cwd[1] = '\0';
        return;
    }

    // when there is something, extract the last one
    for (i = 0; i < MAX_CWD_LEN - 1; ++i) {
        if (dir_seq[i+1] == NULL) {
            strcpy(cwd, dir_seq[i]);
            return;
        }
    }
}


void parse_input_output(char cmd_string[], char input_file[], char output_file[])
{
    char *curr;
    for (curr = cmd_string; *curr != '\0';) {
        if (*curr == '>') {
            *curr = '\0';
            ++curr;
            int i = 0;
            while(*curr != '<' && *curr != '\0') {
                if (*curr == ' ') {
                    ++curr;
                    continue;
                }
                output_file[i] = *curr;

                ++i;
                ++curr;
            }
        } else if (*curr == '<') {
            *curr = '\0';
            ++curr;
            int i = 0;
            while(*curr != '>' && *curr != '\0') {
                if (*curr == ' ') {
                    ++curr;
                    continue;
                }
                input_file[i] = *curr;

                ++i;
                ++curr;
            }
        } else {
            ++curr;
        }
    }
}
