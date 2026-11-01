#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define MAX_ARGS 40

int main(void){
    char input[MAX_LINE];
    char last_command[MAX_LINE] = "";
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];

    int should_run = 1;

    while(should_run){
        printf("osh> ");
        fflush(stdout);

        // Read the command from the user
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';	// Remove the newline character

        // Ignore empty commands
        if (strlen(input) == 0)
            continue;


        // ==================================================
        // PART 1 & 2: COMMAND EXECUTION AND HISTORY FEATURE
        // ==================================================

        // Execute the previous command when the user enters !!
        if(strcmp(input, "!!") == 0){
            if(strlen(last_command) == 0){
                printf("No commands in history.\n");
                continue;
            }
            strcpy(input, last_command);
            printf("%s\n", input);	// Display the command being executed
        }else{
            strcpy(last_command, input);	// Save the current command in history
        }

        // Exit the shell
        if(strcmp(input, "exit") == 0){
            should_run = 0;
            continue;
        }

        // ==================================================
        // PART 3: PARSE COMMANDS AND REDIRECTION
        // ==================================================
        char *input_file = NULL;
        char *output_file = NULL;
        int background = 0;
        int pipe_found = 0;
        int redirection_found = 0;
        int count1 = 0;
        int count2 = 0;
        char *token = strtok(input, " \t");

        while(token != NULL){
            // Check for background execution
            if(strcmp(token, "&") == 0){
                background = 1;
                token = strtok(NULL, " \t");
                continue;
            }
            // Check for pipe
            if(strcmp(token, "|") == 0){
                if(pipe_found || redirection_found){
                    printf("Invalid command.\n");
                    count1 = 0;
                    break;
                }

                pipe_found = 1;
                token = strtok(NULL, " \t");
                continue;
            }

            // Check for input redirection
            if(strcmp(token, "<") == 0){
                if(pipe_found || redirection_found){
                    printf("Invalid command.\n");
                    count1 = 0;
                    break;
                }

                redirection_found = 1;
                input_file = strtok(NULL, " \t");
                if(input_file == NULL){
                    printf("Missing input file.\n");
                    count1 = 0;
                    break;
                }

                token = strtok(NULL, " \t");
                continue;
            }

            // Check for output redirection
            if(strcmp(token, ">") == 0){
                if(pipe_found || redirection_found){
                    printf("Invalid command.\n");
                    count1 = 0;
                    break;
                }

                redirection_found = 1;
                output_file = strtok(NULL, " \t");
                if(output_file == NULL){
                    printf("Missing output file.\n");
                    count1 = 0;
                    break;
                }

                token = strtok(NULL, " \t");
                continue;
            }

            // Store command arguments
            if(!pipe_found)
                args1[count1++] = token;
            else
                args2[count2++] = token;

            token = strtok(NULL, " \t");
        }

        // End the argument arrays with NULL
        args1[count1] = NULL;
        args2[count2] = NULL;

        // Ignore invalid or empty commands
        if(count1 == 0)
            continue;

        // A pipe must have two commands
        if(pipe_found && count2 == 0){
            printf("Missing command after pipe.\n");
            continue;
        }


        // ==================================================
        // PART 4: PIPE COMMUNICATION
        // ==================================================
        if(pipe_found){
            int pipe_fd[2];
            // Create a pipe between the two processes
            if(pipe(pipe_fd) == -1){
                perror("pipe");
                continue;
            }
            pid_t pid1 = fork();	// Create the first child process

            if(pid1 == 0){
                // First child: send output to the pipe
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                execvp(args1[0], args1);
                perror("Command execution failed");
                exit(1);
            }

            // Create the second child process
            pid_t pid2 = fork();

            if(pid2 == 0){
                // Second child: read input from the pipe
                dup2(pipe_fd[0], STDIN_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                execvp(args2[0], args2);
                perror("Command execution failed");
                exit(1);
            }

            // Parent closes its pipe descriptors
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            // Wait for both children unless running in background
            if(!background){
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            }
            continue;
        }


        // ==================================================
        // PART 5: INPUT AND OUTPUT REDIRECTION
        // ==================================================

        // Create a child process for the command
        pid_t pid = fork();

        if(pid < 0){
            perror("fork");
            continue;
        }

        if(pid == 0){		// Child process
            // Redirect input from a file
            if(input_file != NULL){
                int fd = open(input_file, O_RDONLY);

                if(fd < 0){
                    perror("Input file");
                    exit(1);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Redirect output to a file
            if(output_file != NULL){
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(fd < 0){
                    perror("Output file");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(args1[0], args1);			// Execute the command
            perror("Command execution failed");		// This line runs only if execvp fails
            exit(1);
        }else{		// Parent process
            // Wait for the child unless command runs in background
            if(!background)
                waitpid(pid, NULL, 0);
        }
    }
    return 0;
}
