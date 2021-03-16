/**
 * DESCRIPTION: A shell to run 
 * just about everyting you
 * could in a normal terminal
 * - bash commands
 * - execute files
 * - change directory
 * 
 * DATE: March 4, 2021
 * FOR: COMP 340, Operating Systems
 * BY: Isaac Lehman & Ethan Harvey
 * 
 * 
 * COMPILE: gcc -o shell shell.c
 * RUN: ./shell
 * STOP: exit or ctr-c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FILE_SIZE 127

/**
 * change text to red
 */
void red()
{
  printf("\033[1;31m");
}

/**
 * change text to yellow
 */
void yellow()
{
  printf("\033[1;33m");
}

/**
 * change text to white
 */
void reset()
{
  printf("\033[0m");
}

/**
 * Prints colored error msg
 */ 
void print_error(char* msg) {
  yellow(); //Set the text to the color yellow
  printf(">>> ");
  red(); //Set the text to the color red
  printf("ERROR: %s", msg);
  reset(); //Resets the text to default color
}

/**
 * Changes the current working directory to dir_path
 */ 
int shell_change_dir(char *dir_path) {
  return chdir(dir_path);
}


/**
 * Checks to see if a file exists
 */ 
int shell_file_exists(char *file_path) {
  struct stat buf;
  int return_value = stat(file_path, &buf);
  return return_value;
}


/**
 * Checks if file exists on PATH and if so copies the path into file_path
 */ 
int shell_find_file(char *file_name, char *file_path, char file_path_size) {
  // traverse the PATH environment variable to find the absolute path of a file/command
  const char *enviroment_paths_constant = getenv("PATH"); // DO NOT MODIFY, MAKE A COPY
  char *enviroment_paths, *tofree;
  tofree = enviroment_paths = strdup(enviroment_paths_constant);
  
  // Iterate to find file
  char *enviroment_path;
  while((enviroment_path = strsep(&enviroment_paths,":")) != NULL ) {
    char new_path[500]; 
    strcpy(new_path, enviroment_path);
    strcat(new_path, "/");
    strcat(new_path, file_name);

    if (shell_file_exists(new_path) == 0) {

      // check if new_path is able to fit inside of file_path
      if (strlen(new_path) < (int)file_path_size) {
        strcpy(file_path, new_path);
        free(tofree);
        return 0;

      } else {
        print_error("File path exceeded maximum length...");
        free(tofree);
        return -1;

      }

    } 
  }
  free(tofree);
  // File was not found
  return -1;
}


/**
 * Forks a child and executes the given command on 
 * the child process
 * 
 * NOTE:
 * - argv[0] must equal file_path
 * - argv[last_index] must equal NULL
 */ 
int shell_execute(char *file_path, char **argv) {
  pid_t pid;
  pid = fork();
  if ( pid < 0 ) {
      printf("Fork failed!\n");
      return -1;
  } else if (pid == 0 ) {
      execv(file_path, argv);
      printf("Should not be printed\n");
  } else {
      wait(NULL);
      return 0;
  }
  return -1;
}




int main (int argc, char *argv[]) {

  printf("\n--- STARTING OS SHELL ---\n\n");

  //run the shell
  while (1) {
    // allocate memory
    char *user_input = malloc(500 * sizeof(char));
    char *line = malloc(1024 * sizeof(char));
    char *file_path = malloc(FILE_SIZE * sizeof(char));
    char **argz = malloc(500 * sizeof(char));

    // Get user input
    yellow(); //Set the text to the color yellow
    printf(">>> ");
    reset(); //Resets the text to default color
    
    char *tofree = fgets(line, 1024, stdin); // read from the standard input
    if (!tofree) {
      continue;
    }
    

    // parse any args
    int index = 0;
    // Extract the first token
    char * token = strtok(line, " ");
    // loop through the string to extract all other tokens
    while( token != NULL ) {
        if(index == 0) {
          token[strcspn(token, "\n")] = 0; // remove newline
          strcpy(user_input, token);
          argz[index] = token;
        } else {
          token[strcspn(token, "\n")] = 0; // remove newline
          argz[index] = token;
        }

        token = strtok(NULL, " ");
        index += 1;
    }

    // add the null character to argz
    argz[index] = NULL;
    


    if (strcmp(user_input, "exit") == 0) {
      // if exit
      printf("\n--- EXIT ---\n\n");
      free(user_input);
      free(file_path);
      free(argz);
      free(tofree);
      break; // break out of loop
    } else if(strcmp(user_input, "") == 0) {
      // if nothing entered
      free(user_input);
      free(file_path);
      free(argz);
      free(tofree);
      continue; // continue to next iteration
    }

    // Find file
    if (strcmp(user_input, "cd") == 0) {
      shell_change_dir(argz[1]);

    // TODO: Figure out what file_path_size char is for
    } else if (shell_find_file(user_input, file_path, FILE_SIZE) == 0) {
      argz[0] = file_path; // set first arg to file path
      shell_execute(argz[0], argz);

    } else if (shell_file_exists(user_input) == 0){
      shell_execute(user_input, argz);

    } else {
      // if invalid command
      print_error("Command not found...");

    }

    // clean up memory
    free(user_input);
    free(file_path);
    free(argz);
    free(tofree); 
    printf("\n");
  }

}
