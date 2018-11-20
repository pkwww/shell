#include "shell.h"
#include "helper_func.h"
#include "execute_cmd.h"


command_t * tokenize_init_single_cmd(char input[])
{
    // initialization
    command_t *cmd = (command_t *) malloc(sizeof(command_t));
    int i;
    for (i = 0; i < MAX_ARG_NUM; ++i) {
        cmd->args[i] = NULL;
    }
    cmd->args_count = 0;

    // tokenize
    const char delim[] = "[\t ]*";
    char *arg = strtok(input, delim);
    int len = strlen(arg);
    cmd->args[cmd->args_count] = (char *) malloc(len);
    strcpy(cmd->args[cmd->args_count], arg);
    ++cmd->args_count;

    while ( (arg = strtok(NULL, delim)) && cmd->args_count < MAX_ARG_NUM) {
        len = strlen(arg);
        cmd->args[cmd->args_count] = (char *) malloc(len);
        strcpy(cmd->args[cmd->args_count], arg);
        ++cmd->args_count;
    }

    if (strcmp(cmd->args[cmd->args_count - 1], "&") == 0) {
        cmd->bg = TRUE;
        cmd->args[cmd->args_count - 1] = NULL;
    } else {
        cmd->bg = FALSE;
    }

    cmd->read_end_fd = STDIN_FILENO;
    cmd->write_end_fd = STDOUT_FILENO;
    cmd->last = FALSE;

    return cmd;
}

void execute_single(command_t *cmd)
{
    if (strcmp(cmd->args[0], "exit") == 0) {
        exec_exit(cmd);
    } else if (strcmp(cmd->args[0], "cd") == 0) {
        exec_cd(cmd);
    } else if (cmd->bg == TRUE) {
        exec_bg(cmd);
    } else {
        /* exec_fg(cmd); */
    }
}

int set_input_file(char input_file[])
{
    if (strlen(input_file) == 0) {
        return 0;
    }
    int fd = open(input_file, O_RDONLY);
    if (fd < 0) {
        perror(input_file);
        return -1;
    }
    return fd;
}

int set_output_file(char output_file[])
{
    if (strlen(output_file) == 0) {
        return 1;
    }
    int fd = open(output_file, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd < 0) {
        perror(output_file);
        return -1;
    }
    return fd;
}

void execute(command_t *cmd_list[], const int list_size, char input_file[], char output_file[])
{
    // TODO:
    // if exit in middle, do not excute it
    // if exit at last, exec_exit

    // assume foregroud first
    // TODO: handle background
    // Note: bg is for the whole list of cmd, not just one
    // also, if '&' exist in the middle, it causes a parse error
    int i;
    /* for (i = 0; i < MAX_CMD_NUM - 1 && cmd_list[i] != NULL; ++i) { */
    for (i = 0; i < list_size; ++i) {
        if (cmd_list[i]->bg == TRUE && cmd_list[i+1] != NULL) {
            printf("pkwshell: parse error\n");
            return;
        }
    }

    int (*pipes)[2];
    int num_of_pipes = list_size + 1;
    pipes = malloc(num_of_pipes * sizeof(int[2]));
    for (i = 1; i < num_of_pipes - 1; ++i) {
        pipe(pipes[i]);
    }
    pipes[0][0] = pipes[num_of_pipes - 1][0] = 0;
    pipes[0][1] = pipes[num_of_pipes - 1][1] = 1;

    int input_fd = set_input_file(input_file);
    if (input_fd < 0) {
        return;
    } else {
        pipes[0][0] = input_fd;
    }

    int output_fd = set_output_file(output_file);
    if (output_fd < 0) {
        return;
    } else {
        pipes[num_of_pipes - 1][1] = output_fd;
    }

    for (i = 0; i < list_size; ++i) {
        cmd_list[i]->read_end_fd = pipes[i][0];
        cmd_list[i]->write_end_fd = pipes[i + 1][1];
    }

    for (i = 0; i < list_size; ++i) {
        exec_fg(cmd_list[i], pipes, num_of_pipes);
        /* printf("p%d read: %d; write: %d\n", i, cmd_list[i]->read_end_fd, cmd_list[i]->write_end_fd); */
    }
}

int main(int argc, char *argv[])
{
    /* signal(SIGCHLD, child_sig_handler); */

    char input[MAX_INPUT_LEN];
    char cwd[MAX_CWD_LEN];
    char *raw_cmd_str_list[MAX_CMD_NUM];
    char input_file[MAX_CMD_NUM];
    char output_file[MAX_CMD_NUM];
    command_t *cmd_list[MAX_CMD_NUM];
    // initialization
    int i;
    for (i = 0; i < MAX_CMD_NUM; ++i) {
        raw_cmd_str_list[i] = NULL;
        cmd_list[i] = NULL;
        input_file[i] = '\0';
        output_file[i] = '\0';
    }

    while (1) {
        short_cwd(cwd);
        printf("[%s] pkwshell> ", cwd);
        fgets(input, MAX_INPUT_LEN, stdin);
        input[strlen(input) - 1] = '\0';
        if (input[0] == '\0') {
            continue;
        }
        split(raw_cmd_str_list, input, "|");

        int i;
        for (i = 0; i < MAX_CMD_NUM && raw_cmd_str_list[i] != NULL; ++i) {
            /* printf("%s\n", raw_cmd_str_list[i]); */
            if (i == 0) {
                parse_input_output(raw_cmd_str_list[0], input_file, output_file);
            }
            if (i == MAX_CMD_NUM-1 || raw_cmd_str_list[i+1] == NULL) {
                parse_input_output(raw_cmd_str_list[i], input_file, output_file);
            }
            cmd_list[i] = tokenize_init_single_cmd(raw_cmd_str_list[i]);
        }
        /* print_cmd_list(cmd_list); */
        /* printf("input file: %s\n", input_file); */
        /* printf("output file: %s\n", output_file); */
        cmd_list[i-1]->last = TRUE;

        execute(cmd_list, i, input_file, output_file);

        clean_command_list(cmd_list);
        for (i = 0; i < MAX_CMD_NUM; ++i) {
            raw_cmd_str_list[i] = NULL;
            input_file[i] = '\0';
            output_file[i] = '\0';
        }
    }
    return 0;
}
