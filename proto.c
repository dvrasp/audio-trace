#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reg.h>
#include <stdio.h>

void trace(pid_t child, FILE *f) {
  unsigned int eip;
  int status;
  while (1) {
    eip = ptrace(PTRACE_PEEKUSER,
                 child, 4 * (EIP),
                 NULL);
    //printf("eip=%x\n", eip);
    fwrite(&eip, 4, 1, f);
    ptrace(PTRACE_SINGLESTEP, child, 0, 0);
    wait(&status);
    if(WIFEXITED(status)) break;
  }
}

int main(int c, char **v)
{   
  pid_t child;
  FILE *f;
  child = fork();
  if(child == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(v[1], &v[1]);
  }
  else {
    wait(NULL);
    f = fopen("log", "w");
    trace(child, f);
    fclose(f);
  }
  return 0;
}
