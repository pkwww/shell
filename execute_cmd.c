#include "execute_cmd.h"
#include "helper_func.h"

void exec_exit(command_t *cmd)
{
    if (cmd->bg == TRUE) {
        return;
    }
    // TODO: how about the child?
    /* clean_command(cmd); */
    exit(EXIT_SUCCESS);
}

void exec_cd(command_t *cmd)
{
    if (cmd->bg == TRUE) {
        return;
    }

    char *path;
    if (cmd->args[1] == NULL) {
        // if no argument provide, cd to $HOME
        path = getenv("HOME");
    } else {
        path = cmd->args[1];
    }
    int ret = chdir(path);

    if (ret < 0) {
        perror(cmd->args[0]);
    }
}

void child_sig_handler(int sig)
{
    int status;

    /* pid_t child_pid = waitpid(-1, &status, WNOHANG); */
    pid_t child_pid;
    while((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("\nchild [%d] terminated with status %d\n", child_pid, status);

        char cwd[MAX_CWD_LEN];
        short_cwd(cwd);
        printf("[%s] pkwshell> ", cwd);
        fflush(stdout);
    }
}

void exec_bg(command_t *cmd)
{
    pid_t pid = fork();
    if (pid == 0) {
        // child
        int ret = execvp(cmd->args[0], cmd->args);
        if (ret < 0) {
            const int err_save = errno;
            if (err_save == ENOENT) {
                fprintf(stderr, "%s: Command not found.\n", cmd->args[0]);
            } else {
                fprintf(stderr, "%s: %s\n", cmd->args[0], strerror(err_save));
            }
        }
    } else if (pid > 0) {
        printf("child [%d] in background\n", pid);
    } else {
        perror("fork error");
    }
}

void close_unrelavent_fd(int read_fd, int write_fd, int (*pipes)[2], int num_of_pipes)
{
    int i;
    for (i = 0; i < num_of_pipes; ++i) {
        if (pipes[i][0] != read_fd) {
            close(pipes[i][0]);
        }
        if (pipes[i][1] != write_fd) {
            close(pipes[i][1]);
        }
    }
}

void exec_fg(command_t *cmd, int (*pipes)[2], int num_of_pipes)
{
    int orig_stdin = 0;
    int orig_stdout = 1;
    pid_t pid = fork();
    if (pid == 0) {
        // child
        close_unrelavent_fd(cmd->read_end_fd, cmd->write_end_fd, pipes, num_of_pipes);
        if (cmd->read_end_fd != 0) {
            /* printf("modify read\n"); */
            /* close(cmd->other_fd); */
            orig_stdin = dup(0);
            dup2(cmd->read_end_fd, 0);
        }
        if (cmd->write_end_fd != 1) {
            /* printf("modify write\n"); */
            /* close(cmd->other_fd); */
            orig_stdout = dup(1);
            dup2(cmd->write_end_fd, 1);
        }

        int ret = execvp(cmd->args[0], cmd->args);
        if (ret < 0) {
            const int err_save = errno;
            if (err_save == ENOENT) {
                fprintf(stderr, "%s: Command not found.\n", cmd->args[0]);
            } else {
                fprintf(stderr, "%s: %s\n", cmd->args[0], strerror(err_save));
            }
            dup2(orig_stdin, 0);
            dup2(orig_stdout, 1);
            exit(-1);
        }
    } else {
        if (cmd->last == TRUE) {
            /* close(cmd->read_end_fd); */
            /* close(cmd->other_fd); */
            close_unrelavent_fd(0, 1, pipes, num_of_pipes);
            waitpid(pid, NULL, 0);
        }
    }
}
