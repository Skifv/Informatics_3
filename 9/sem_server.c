#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#if 0
#define prn fprintf(stderr, "%d: %s\n", __LINE__, __func__);
#else
#define prn
#endif

#define SERVER 1
#define N 100

#define V 1
#define P 2

/* Схема клиент-сервер */

    struct client 
    {
        long mtype;              // всегда SERVER   
        struct {
            long id;             // id процесса-клиента

            unsigned int sem_op; // V - V(S), P - P(S)
            unsigned int * sem;  // указатель на разделяемую память с семафором
        } message;
    };

    struct server 
    {
        long mtype;
        struct 
        {
            int nothing; // не отвечает ничего, длина 0
        } message;
   };

int get_message_queue(char *pathname, int project_id);

int main(void)
{
    char pathname[]="sem_server.c";
    int project_id = 0;

    // создание очереди сообщений
    int msqid = get_message_queue(pathname, project_id);

    // инициализация сообщения от клиента
    struct client client_msg;

    // ответ сервера
    struct server server_msg;   
    
    printf("Server is on, waiting requests.\n");

    while(1)
    {
        // получение сообщения
        if((msgrcv(msqid, (struct msgbuf *) &client_msg, sizeof(client_msg.message), SERVER, 0)) < 0){
            perror("msgrcv");
            exit(-1);
        }

        if (client_msg.message.sem_op == V) {

            printf("Got request from client %ld: V(S)\n", client_msg.message.id, client_msg.message.request);

            // увеличиваем семафор
            client_msg.message->sem += 1;
            
            // отправляем пустое подтверждающее сообщение
            if ((msgsnd(msqid, (struct msgbuf *) &server_msg, sizeof(server_msg.message), 0)) < 0){
                perror("msgsnd");
                exit(-1);
            }

            printf("Request from client %ld processed. \n", client_msg.message.id);

            // если в очереди семафоров есть ожидающий процесс клиента client_msg.message.id с P(client_msg.message->sem), то отправляем ему сообщение
    
        }
        else if (client_msg.message.sem_op == P) {

            printf("Got request from client %ld: P(S)\n", client_msg.message.id, client_msg.message.request);

            if (client_msg.message->sem > 0) {

                // уменьшаем семафор
                client_msg.message->sem -= 1;
                
                // отправляем пустое подтверждающее сообщение
                if ((msgsnd(msqid, (struct msgbuf *) &server_msg, sizeof(server_msg.message), 0)) < 0){
                    perror("msgsnd");
                    exit(-1);
                }

                printf("Request from client %ld processed. \n", client_msg.message.id);
            }
            else if (client_msg.message->sem == 0) {

                // помещаем этот клиент client_msg.message.id в очередь семафоров
                
            }
        }

        
    }

    return 0;    
}


int get_message_queue(char *pathname, int project_id)
{
    key_t key = ftok(pathname, project_id);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    return msqid;
}

