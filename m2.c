#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

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


void parseData(char bufor[128], int bytesRead, int numbers[6]){
    int numIndex = 0;
    int currentNumber = 0;
    int isNumber = 0; 
    for (int i = 0; i < bytesRead; i++){
        if (isdigit(bufor[i])){
            currentNumber = currentNumber * 10 + (bufor[i] - '0');
            isNumber = 1;
        } else if (isNumber){
            numbers[numIndex] = currentNumber;
            numIndex++;
            currentNumber = 0;
            isNumber = 0;
        }
    }

    if (isNumber){
        numbers[numIndex] = currentNumber;
        numIndex++;
    }

}

int main(int argc, char *argv[]){
    int fd;
    char bufor[128];
    int bytesRead;
    int numbers[6];
    int surowiecA, surowiecB, surowiecC, aPrice, bPrice, cPrice; 

    fd = open(argv[1], O_RDONLY);
    if (fd<0){
        perror("Nie można odczytać pliku");
        return 1;
    }

    bytesRead = read(fd, bufor, sizeof(bufor));
    if (bytesRead < 0){
        perror("Błąd odczytu pliku");
        return 1;
    }

    close(fd);
    
    parseData(bufor, bytesRead, numbers);
    surowiecA = numbers[0];
    surowiecB = numbers[1];
    surowiecC = numbers[2];
    aPrice = numbers[3];
    bPrice = numbers[4];
    cPrice = numbers[5];
    int liczbaKurierow = 3;
    printf("A: %d, B: %d, C: %d, aPrice: %d, bPrice: %d, cPrice: %d\n", surowiecA, surowiecB, surowiecC, aPrice, bPrice, cPrice);

    struct msgbuf zamowienie;
    key_t key = atoi(argv[2]);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Nie można utworzyć kolejki komunikatów");
        return 1;
    }

    int msgid2 = msgget(5, 0666 | IPC_CREAT);
    if (msgid2 == -1){
        perror("Nie można utworzyć kolejki komunikatów");
        return 1;
    }

   struct msgbuf2 magazyn;


    int pid = fork();
    if (pid>0){
        int pid2 = fork();
        if(pid2>0){
            int pid3 = fork();
            if(pid3>0){

                while (1) {
                    msgrcv(msgid2, &zamowienie, sizeof(zamowienie), 2, 0);
                    printf("Magazyn otrzymał zamówienie nr %d: A: %d, B: %d, C: %d\n", zamowienie.id, zamowienie.A, zamowienie.B, zamowienie.C);

                        if (zamowienie.A <= surowiecA && zamowienie.B <= surowiecB && zamowienie.C <= surowiecC) {
                            magazyn.gold = zamowienie.A * aPrice + zamowienie.B * bPrice + zamowienie.C * cPrice;
                            
                            surowiecA -= zamowienie.A;
                            surowiecB -= zamowienie.B;
                            surowiecC -= zamowienie.C;
                            
                            magazyn.mtype = 2;
                            msgsnd(msgid2, &magazyn, sizeof(magazyn), 0);
                            
                            alarm(180);
                        } else {
                            printf("Magazyn nie ma wystarczających zasobów na zamówienie nr %d\n", zamowienie.id);
                            kill(zamowienie.id, SIGKILL);
                            liczbaKurierow--;
                            if (liczbaKurierow == 0){
                                // msgctl(1, IPC_RMID, NULL);
                                // msgctl(key, IPC_RMID, NULL);
                                printf("Surowiec A: %d, Surowiec B: %d, Surowiec C: %d\n", surowiecA, surowiecB, surowiecC);
                                kill(getpid(), SIGKILL);
                                return 0;
                            }
                        }
                       
                }
            }else{
                while (1)
                {
                    msgrcv(msgid, &zamowienie, sizeof(zamowienie), 1, 0);
                    printf("Zamówienie nr %d odebrane przez kuriera 3: A: %d, B: %d, C: %d\n", zamowienie.id, zamowienie.A, zamowienie.B, zamowienie.C);
                    zamowienie.mtype = 2;
                    zamowienie.id = getpid();
                    msgsnd(msgid2, &zamowienie, sizeof(zamowienie), 0);
                    msgrcv(msgid2, &magazyn, sizeof(magazyn), 2, 0);
                    printf("Kurier 3 odebrał złoto: %d\n", magazyn.gold);
                    msgsnd(msgid, &magazyn, sizeof(magazyn), 0);

                }
                
            }
        }else{
                while (1)
                {
                    msgrcv(msgid, &zamowienie, sizeof(zamowienie), 1, 0);
                    printf("Zamówienie nr %d odebrane przez kuriera 3: A: %d, B: %d, C: %d\n", zamowienie.id, zamowienie.A, zamowienie.B, zamowienie.C);
                    zamowienie.mtype = 2;
                    zamowienie.id = getpid();
                    msgsnd(msgid2, &zamowienie, sizeof(zamowienie), 0);
                    msgrcv(msgid2, &magazyn, sizeof(magazyn), 2, 0);
                    printf("Kurier 2 odebrał złoto: %d\n", magazyn.gold);
                    msgsnd(msgid, &magazyn, sizeof(magazyn), 0);
                }
        }
    }else{
            while (1)
                {
                    msgrcv(msgid, &zamowienie, sizeof(zamowienie), 1, 0);
                    printf("Zamówienie nr %d odebrane przez kuriera 3: A: %d, B: %d, C: %d\n", zamowienie.id, zamowienie.A, zamowienie.B, zamowienie.C);
                    zamowienie.mtype = 2;
                    zamowienie.id = getpid();
                    msgsnd(msgid2, &zamowienie, sizeof(zamowienie), 0);
                    msgrcv(msgid2, &magazyn, sizeof(magazyn), 2, 0);
                    printf("Kurier 1 odebrał złoto: %d\n", magazyn.gold);
                    msgsnd(msgid, &magazyn, sizeof(magazyn), 0);
                }
    }

   
    return 0;
}
