// EECS 370 Lab 1
// YOU MAY MODIFY THIS FILE BUT DON'T SUBMIT

#include <stdio.h>
#include <stdlib.h>
#include "lab1.h"

int main() {
    if (extract(0x2020) != 0b0010) {
        printf("Extract Test failed :(\n");
        exit(1);
    }
    printf("Passed! :)\n");
}