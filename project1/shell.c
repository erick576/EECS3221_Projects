
/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Project 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE   	 80 /* 80 chars per line, per command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];    /* command line (of 80) has max of 40 arguments */
    char *args_pipe[MAX_LINE/2 + 1]; // Split commands for pipelining
    char history[MAX_LINE];
    char input_string[MAX_LINE];
    int should_run = 1;
    pid_t pid, pid1;
    int in = 0;
    int out = 0;
    int has_history = 0;
    int is_pipe = 0;
    int fd_pipe[2];

    while (should_run){
        printf("mysh:~$ ");
        fflush(stdout);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */

        // Read the input arguments to see if they meet the max length requirements and store it
        if (!fgets(input_string, MAX_LINE, stdin)) return 0;
        input_string[strcspn(input_string, "\n")] = '\0'; // Remove the linespace that fgets adds to the input

        // If input was given into the shell then add it to the history
        if (strcmp(input_string, "!!") != 0)
        {
            has_history = 1;
            strcpy(history, input_string);
        }

        // If the history command was used then copy it onto the current input
        if (strcmp(input_string, "!!") == 0 && has_history == 1)
        {
            printf("%s\n", history);
            strcpy(input_string, history);
        }

        // If no processes yet then provide context
        if (strcmp(input_string, "!!") == 0 && has_history == 0)
        {
            printf("No commands in history.\n");
            continue;
        }

        // Look for inpiut/output redirection
        for (int i = 0; i < strlen(input_string); i++) {
            if (input_string[i] == '<')
            {
                input_string[i] = ' ';
                in = 1;
            }
            else if (input_string[i] == '>')
            {
                input_string[i] = ' ';
                out = 1;
            }
            else if (input_string[i] == '|')
            {
                is_pipe = 1;
            }
        }

        // Create a pointer to store the split input argument in order to separate the arguments
        char * stdin_token = strtok(input_string, " ");
        int i = 0;
        int j = 0;

        // Pass the split words into our arguments
        int break_off = 0;
        while (stdin_token != NULL)
        {
            // Split arguments if there is pipelining
            if (is_pipe == 1)
            {
                if (stdin_token != NULL && strcmp(stdin_token, "|") == 0)
                {
                    break_off = 1;
                    stdin_token = strtok(NULL, " ");
                    continue;
                }
                if (break_off == 0){
                    args[i++] = stdin_token;
                    stdin_token = strtok(NULL, " ");
                }
                else
                {
                    args_pipe[j++] = stdin_token;
                    stdin_token = strtok(NULL, " ");
                }
            }
            else
            {
                args[i++] = stdin_token;
                stdin_token = strtok(NULL, " ");
            }
        }
        args[i] = NULL;
        if (is_pipe == 1)
        {
            args_pipe[j] = NULL;
        }

        // If the user enters exit at the prompt then we should set should_run to 0 and terminate
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            return 0;
        }
        else
        {
            wait(NULL);

            if (strcmp(args[i - 1], "&") == 0)
            {
                args[i - 1] = NULL;
                i --;

                // We will now create a new child process concurrently
                pid = fork();

                if (pid < 0)
                {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }
                else if (pid == 0) // Child Process
                {
                    int fd_0 = open("/dev/null", O_WRONLY);
                    dup2(fd_0, 1);
                    dup2(fd_0, 2);
                    close(fd_0);
                    close(0);
                    execvp(args[0], args);
                    _exit(0);
                }
                else // Parent Process
                {
                    // Parent will not invoke wait
                }
            }
            else
            {
                // We will now create a new child process not concurrently
                if (is_pipe == 1)
                {
                    if (pipe(fd_pipe) == -1)
                    {
                        fprintf(stderr, "Pipe Failed");
                        return 1;
                    }

                    pid = fork();

                    if (pid < 0)
                    {
                        fprintf(stderr, "Fork Failed");
                        return 1;
                    }

                    if (pid > 0)
                    {
                        pid1 = fork();

                        if (pid1 < 0)
                        {
                            fprintf(stderr, "Fork Failed");
                            return 1;
                        }
                        if (pid1 > 0)
                        {
                            close(fd_pipe[0]);
                            close(fd_pipe[1]);
                            wait(NULL);
                            wait(NULL);
                        }
                        else
                        {
                            close(fd_pipe[1]);
                            dup2(fd_pipe[0], STDIN_FILENO);
                            close(fd_pipe[0]);

                            execvp(args_pipe[0], args_pipe);
                            _exit(0);
                        }
                    }
                    else
                    {
                        close(fd_pipe[0]);
                        dup2(fd_pipe[1], STDOUT_FILENO);
                        close(fd_pipe[1]);

                        execvp(args[0], args);
                        _exit(0);
                    }
                }
                else
                {
                    pid = fork();

                    if (pid < 0)
                    {
                        fprintf(stderr, "Fork Failed");
                        return 1;
                    }
                    else if (pid == 0) // Child Process
                    {
                        // Check for input redirection
                        if (in == 1)
                        {
                            int fd = open(args[i - 1], O_RDONLY);
                            dup2(fd, STDIN_FILENO);
                            close(fd);
                            args[i - 1] = NULL;
                        }

                        // Check for output redirection
                        if (out == 1)
                        {
                            int fd = creat(args[i - 1], 0644);
                            dup2(fd, STDOUT_FILENO);
                            close(fd);
                            args[i - 1] = NULL;
                        }

                        if (strcmp(args[0], "cd") == 0)
                        {
                            if (args[1] != NULL && chdir(args[1]) != 0)
                            {
                                printf("-bash: cd: %s: No such file or directory\n", args[1]);
                            }
                        }

                        execvp(args[0], args);
                        _exit(0);
                    }
                    else // Parent Process
                    {
                        wait(NULL);
                    }
                }
            }
        }
        in = 0;
        out = 0;
        is_pipe = 0;
    }

    return 0;
}
