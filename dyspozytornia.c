#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

struct msgbuf{
    long mtype;
    int id;
    int A;
    int B;
    int C;
};

int main(int argc, char *argv[]){
    key_t key = atoi(argv[1]);
    int liczbaZamowien = atoi(argv[2]);
    int maxAPerZam = atoi(argv[3]);
    int maxBPerZam = atoi(argv[4]);
    int maxCPerZam = atoi(argv[5]);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Nie można utworzyć kolejki komunikatów");
        return 1;
    }

struct msgbuf zamowienie;

    for (int i = 0; i < liczbaZamowien; i++){
        int A = rand() % maxAPerZam + 1;
        int B = rand() % maxBPerZam + 1;
        int C = rand() % maxCPerZam + 1;

        zamowienie.mtype = 1;
        zamowienie.id=i;
        zamowienie.A = A;
        zamowienie.B = B;
        zamowienie.C = C;
        msgsnd(msgid, &zamowienie, sizeof(zamowienie), 0);

        printf("Zamówienie nr %d: A: %d, B: %d, C: %d\n", i, A, B, C);
        sleep(1);
    }

    printf("key: %d\n", key);

    return 0;
}