#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <string.h>
#include  <assert.h>
#include  <sys/wait.h>


/* run the command: "ls -al | nl" to test ipc pipe */

int main()
{
    char * const command_ls[] = {"ls", "-al", 0};
    char * const command_nl[] = {"nl", 0};
    pid_t child_ls, child_nl;
    // pipe_fd[0] for read and pipe_fd[1] for write
    int pipe_fd[2];
    pipe(pipe_fd);
    if( (child_nl = fork()) < 0 )
    {
        printf("strerror: %s\n",strerror(errno));
        perror("perror");
        assert(child_nl >= 0);
    }
    if( child_nl == 0 )
    {
        //child_nl is for read
        close(pipe_fd[1]);
        //redirect the stdin to pipe_fd[0]
        close(STDIN_FILENO);
        dup2(pipe_fd[0],STDIN_FILENO);
        close(pipe_fd[0]);

        execvp("nl",command_nl);
        printf("child_nl gets wrong!\n");
        exit(-1);
    }
    //child_ls is for write
    close(pipe_fd[0]);
    if( (child_ls = fork()) < 0 )
    {
        printf("strerror: %s\n",strerror(errno));
        perror("perror");
        assert(child_ls >= 0);
    }
    if( child_ls == 0 )
    {
        //redirect stdout to pipe_fd[1]
        close(STDOUT_FILENO);
        dup2(pipe_fd[1],STDOUT_FILENO);
        close(pipe_fd[1]);

        execvp("ls",command_ls);
        printf("child_ls gets wrong!\n");
        exit(-1);
    }

    close(pipe_fd[1]);
    waitpid(child_nl,NULL,0);
    return 0;
}
