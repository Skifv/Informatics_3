#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
int main()
{
pid_t pid, ppid;
int a = 0;
(void)fork();

a = a+1;

pid = getpid();
ppid = getppid();

printf("My pid = %d, my ppid = %d, result = %d\n",
(int)pid, (int)ppid, a);
return 0;
}