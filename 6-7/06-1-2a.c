#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define N 100

int main() 
{ 
   char *  array;   // передаваемая строка
   int     shmid;   // возвращаемое значение shmget
   char    pathname[] = "06-1a.c"; 
   key_t   key;     // ключ для файла

   if((key = ftok(pathname,0)) < 0){
     printf("Can\'t generate key\n");
     exit(-1);
   }

   if((shmid = shmget(key, N*sizeof(char), 0666|IPC_CREAT)) < 0){
    printf("Can\'t create shared memory\n");
    exit(-1);
   }

   if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)){
      printf("Can't attach shared memory\n");
      exit(-1);
   }

   memcpy(array, "Hello, Vladislav!", strlen("Hello, Vladislav!") + 1);

   if(shmdt(array) < 0){
      printf("Can't detach shared memory\n");
      exit(-1);
   }

   return 0;
}
