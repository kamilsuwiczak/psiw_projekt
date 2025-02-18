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

struct msgbuf2{
    long mtype;
    int gold;
};


int main(int argc, char *argv[]){
    key_t key = atoi(argv[1]);
    int liczbaZamowien = atoi(argv[2]);
    int maxAPerZam = atoi(argv[3]);
    int maxBPerZam = atoi(argv[4]);
    int maxCPerZam = atoi(argv[5]);

    int gold = 0;

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Nie można utworzyć kolejki komunikatów");
        return 1;
    }

struct msgbuf zamowienie;
struct msgbuf2 goldMsg;

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
    while(1){
        msgrcv(msgid, &goldMsg, sizeof(goldMsg), 2, 0);
        gold += goldMsg.gold;
        // if (msgget(key, 0666)<0){
        //     break;    
        // }
        printf("Złoto: %d\n", gold);
    }
    
    

    return 0;
}