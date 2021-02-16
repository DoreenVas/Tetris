#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include <stdio.h>

#define ERROR -1

void writeError();
char getch();

/***
 * The function creates a son with fork. The son uses exec and runs draw.out which is ex52.c
 * The father receives chars from the keyboard and sends them to the son via IO Redirection and Pipeline.
 * When 'q' is received the program ends.
 * @return 0 for success
 */
int main() {
    int value, origStdout;
    int fd[2];
    origStdout = dup(1); //Save our original stdout

    if (pipe(fd) < 0) {
        writeError();
        return ERROR;
    }
    pid_t pid = fork();
    if(pid < 0){
        writeError();
        return ERROR;
    }
    else if (pid == 0) { //in child
        dup2(fd[0], STDIN_FILENO); //Force our stdin to be the read side of the pipe we made
        dup2(origStdout, STDOUT_FILENO);//Force our stdout to be the original standard out

        //Close off all the pipes we no longer need
        close(origStdout);
        close(fd[0]);
        close(fd[1]);

        char *args[] = {"./draw.out", NULL};
        value = execvp(args[0], &args[0]);
        if (value == -1) {
            writeError();
            return ERROR;
        }
    } else { //in parent
        dup2(fd[1], STDOUT_FILENO);//Force our stdout to be the write end of the Pipe we formed

        //Close off all the pipes we no longer need
        close(origStdout);
        close(fd[0]);
        close(fd[1]);

        char ch=getch();
        while (ch != 'q'){

            if(ch == 'a' || ch == 's' ||ch == 'd' ||ch == 'w'){
                write(STDOUT_FILENO,&ch,sizeof(ch));
                if(kill(pid,SIGUSR2) == ERROR)
                    writeError();
            }
            ch=getch();
        }
        write(STDOUT_FILENO,&ch,sizeof(ch));
        if(kill(pid,SIGUSR2) == ERROR)
            writeError();
    }
    return 0;
}

/***
 * Writes an Error message to STDERR.
 */
void writeError() {
    char *error = "Error in system call";
    write(STDERR_FILENO, error, sizeof(error));
}

/***
 * Gets char from keyboard without pressing Enter.
 * @return the char received.
 */
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}
