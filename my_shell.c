// learning how to make a simple shell using C

//header files

#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <termios.h>


// defines

#define SH_RL_BUFSIZE 1024
#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"
#define HISTORY_SIZE 3

//char *sh_read_line(void){
//	int bufsize = SH_RL_BUFSIZE; // initial block size
//	int position = 0; // will be used in looping of array
//	char *buffer = malloc(sizeof(char) * bufsize); // allocating the inital block memory
//	int c;
//
//	if (!buffer) {
//	fprintf(stderr, "sh: allocation error! \n");
//	exit(EXIT_FAILURE);
//	}
//
//	while(1){
//		c = getchar(); //reads char
//		if (c == EOF || c == '\n') { // if c = getchar() returns EOF then error
//			buffer[position] = '\0'; // we replace \0 to that buffer position
//			return buffer;
//		} else {
//			buffer[position] = c; // if successful, then store those chars in buffer
//		}
//		position++; // updating and increasing size to capture all chars in buffer
//
//		if (position >= bufsize) { // if buffer size not enough for args
//		bufsize += SH_RL_BUFSIZE; // increase +1024 more
//		buffer = realloc(buffer, bufsize); //realloc more mem for buffer
//			if (!buffer) { // incase of error at realloc
//				fprintf(stderr, "sh: allocation error\n");
//				exit(EXIT_FAILURE);
//			}
//		}
//	}
//}

// another method (faster)

char *history[HISTORY_SIZE]; // array where we store commands
int history_count = 0; // to track cmnds stored in array
int history_index = -1;

// CONTINUE FROM HERE!

void add_to_history(char *command) { // arguments -> a pointer to a string array
	if (history_count < HISTORY_SIZE) {
		history[history_count++] = strdup(command); //history is a pointer to a string and adds commands
	} else {
		free(history[0]); // if size more then we free the oldest history [0]
		for (int i = 1; i < HISTORY_SIZE; i++) { // resetting index 1 at 0, 2 at 1, so we allocate the latest space for cmnd
			history[i-1] = history[i];
		}

		history[HISTORY_SIZE - 1] = strdup(command); // allocating the latest command to last index of array
	}
}

void enable_raw_mode(struct termios *orig_termios) { // function that takes in termios struct which is a config of terminal
	struct termios raw; //raw is where we take config of stdin_fileno (our terminal)
	tcgetattr(STDIN_FILENO, orig_termios); // get attributes of shell and store in orig_termios
	raw = *orig_termios; // store orig-termios data in raw to not modify the orig_termios but raw only
	raw.c_lflag &= ~(ICANON | ECHO); // disabling canonical and echo mode to read arrow input in raw
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); //set attribute of shell from raw (not orig_termios)
}

void disable_raw_mode(struct termios *orig_termios) { // to stop reading arrow input
	tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios); // set the shell attibutes back to orig_termois (raw was modified)
}

//function to detect arrow keys and handle history traversal

//void handle_keypress(char **input_buffer) {
//	char c;
//	struct termios orig_termios;
//	enable_raw_mode(&orig_termios);
//
//	while(1){
//		c = getchar();
//		if (c == '\033') {
//			switch(getchar()) {
//				case 'A' :
//					if (history_count > 0 && history_index > 0)  {
//						history_index--;
//						printf("\33[2K\r> %s", history[history_index]);
//						strcpy(*input_buffer, history[history_index]);
//					} else if (history_index == -1) {
//						history_index = history_count - 1;
//						printf("\33[2K\r> %s",  history[history_index]);
//						strcpy(*input_buffer, history[history_index]);
//					}
//					break;
//				case 'B' :
//					if  (history_index <  history_count - 1) {
//						history_index++;
//						printf("\33[2K\r> %s", history[history_index]);
//						strcpy(*input_buffer, history[history_index]);
//					} else {
//						printf("\33[2K\r> ");
//						history_index = -1;
//						(*input_buffer)[0] = '\0';
//					}
//					break;
//
//			} else if (c == '\n') {
//				break;
//			} else {
//				putchar(c);
//				strncat(*input_buffer, &c, 1);
//			}
//		}
//	disable_raw_mode(&orig_termios);
//}

char *sh_read_line(void) { // we used getLine before, now we'll capture chars from c one by one & raw mode disabled

	char *line = malloc(1024); // holds all chars received from stdin
	int bufsize = 1024; // buffer size, will start at 0 but getLine will dynamically increase it as per stdin

	// getline(store the chars, buffer size, stdin ) [for my understanding]

	//if (getline(&line, &bufsize, stdin) ==  -1) { // getline return either 1 or -1, if -1 means error or EOF
	//	if (feof(stdin)) { // if reached EOF
	//		exit(EXIT_SUCCESS); // exit
	//	} else {
	//		perror("readline"); //  if not EOF then error found
	//		exit(EXIT_FAILURE); // exit
	//	}
	//}


	struct termios orig_termios;
	enable_raw_mode(&orig_termios);

	int position = 0;
	char c;

	line[0] = '\0';

	while(1) {
		c = getchar();
		if (c == '\033') {
			getchar();
			switch(getchar()) {
				case 'A' :
					if (history_index > 0  && history_count > 0) {
						history_index--;
						printf("\33[2K\r> %s", history[history_index]);
						strcpy(line, history[history_index]);
						position = strlen(line);
					} else if (history_index = -1) {
						history_index = history_count - 1;
						printf("\33[2k\r> %s", history[history_index]);
						strcpy(line, history[history_index]);
						position = strlen(line);
					}
					break;
				case 'B':
					if (history_index < history_count - 1) {
						history_index++;
						printf("\33\[2k\r> %s", history[history_index]);
						strcpy(line, history[history_index]);
						position = strlen(line);
					} else {
						printf("\33[2K\r> ");
						line[0] = '\0';
						position = 0;
						history_index = -1;
					}
					break;
				case 'D':
					if (position > 0) {
						position--;
						printf("\b");
					}
					break;
				case 'C':
					if (position < strlen(line)) {
						position++;
						printf("\033[C");
					}
					break;
			}
		}
		else if (c == '\n') {
			line[position] = '\0';
			break;
		}
		else if (c == 127) {
			if (position > 0) {
				position--;
				printf("\b \b");
				line[position] = '\0';
			}
		}
		else {
			if (position >= bufsize - 1) {
				bufsize+=1024;
				line = realloc(line, bufsize);
			}

			line[position] = c;
			position++;
			line[position] = '\0';
			putchar(c);
		}
	}

	disable_raw_mode(&orig_termios);

	add_to_history(line);

	return line; // return line as it contains all the chars from stdin
}

char **sh_split_line(char *line) {
	int bufsize = SH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "sh: allocation error!\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, SH_TOK_DELIM);
	while(token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize+=SH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "sh: allocation error!\n");
				exit(EXIT_FAILURE);
			}
		}
	token = strtok(NULL, SH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}


int sh_launch(char **args) {
	pid_t pid, wpid; // process ids type
	int status;

	pid = fork(); // creation of child process
	if (pid == 0) { // child process part
		if (execvp(args[0], args) == -1) { // execvp like exec except it takes command name and array
			perror("sh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) { // if pid had error in fork init
		perror("sh");
	}
	else { // parent part while child is processing
		do {
			wpid = waitpid(pid, &status, WUNTRACED); // keep on calling waitpid to check if child status changed
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); // child status changed due to signal or exited
	}
	printf("launch");
	return 1;

}


// builtin shell commands

int sh_cd(char **args);
int sh_exit(char **args);
int sh_help(char **args);
int sh_pwd(char **args);



char *builtin_str[] = { // a pointer array that is used to checking stdin
	"cd",
	"exit",
	"help",
	"pwd"
};

// this is an arrow function that returns int and forces arguments on its elements as double pointer char

int (*builtin_func[]) (char **)  = { // builtin_func->array having a pointer to func that takes char ** as argument and returns int
	&sh_cd, // these are used so that once we match the command from stdin we can redirect them to the functions by Dynamic MA
	&sh_exit,
	&sh_help,
	&sh_pwd
};

int sh_num_builtins() { // to find the length of the builtin_str array
	return sizeof(builtin_str) / sizeof (char *); // sizeof builtin_str = 3 * 8 / sizeof one char = 8 i.e 24/8 = 3
}


int sh_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "sh: expected argument!");
	} else {
		if (chdir(args[1]) != 0) {
			perror("sh");
		}
	}
	return 1;
}

int sh_help(char **args) {
	printf("db's custom shell! \n");
	printf("try all these commands below and have a look around!\n");
	printf("the following are the current builtin commands:\n");
	for (int i = 0; i < sh_num_builtins(); i++){
		printf("%s\n", builtin_str[i]);
	}
	printf("try using man for more info!\n");
	return 1;
}

int sh_pwd(char **args) {
	char cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) !=  NULL) {
		printf("current working dir: %s\n", cwd);
	}
	else{
		perror("sh: couldnt fetch dir");
	}

	return 1;
}


int sh_exit(char **args) {
	return 0;
}


int sh_execute(char **args) {

	if (args[0] == NULL) {
                return 1;
        }

	char *str = args[0];

	for (int i = 0; str[i] != '\0'; i++){
		str[i] = tolower(str[i]);
	}

	for (int i = 0; i < sh_num_builtins(); i++){
		if (strcmp(args[0], builtin_str[i]) == 0) {
			printf("exec init");
			return (*builtin_func[i])(args);
		}
	}

	return sh_launch(args);
}



//main loop of our code

void sh_loop(void){
        char *line;
        char **args;
        int status;

        do {
        printf("> ");
        line = sh_read_line(); // read the arg line we type out
        args = sh_split_line(line); // split the line using strtok 
        status = sh_execute(args); // will check the [0] arg and execute [1] from line

        free(line); // free line memory
        free(args); // free args memory
        } while (status); // loop continues unless status is NULL
}


// main function of code

int main(int argc, char **argv) {
        sh_loop();  // we call the main loop of our shell

        return EXIT_SUCCESS;
}
