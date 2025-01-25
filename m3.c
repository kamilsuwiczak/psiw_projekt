#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

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
    int A, B, C, aPrice, bPrice, cPrice; 

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
    A = numbers[0];
    B = numbers[1];
    C = numbers[2];
    aPrice = numbers[3];
    bPrice = numbers[4];
    cPrice = numbers[5];
    printf("A: %d, B: %d, C: %d, aPrice: %d, bPrice: %d, cPrice: %d\n", A, B, C, aPrice, bPrice, cPrice);



    return 0;
}
