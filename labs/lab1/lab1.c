// EECS 370 Lab 1
// MODIFY AND SUBMIT THIS FILE
#include "lab1.h"

/*  Write a function int extract(int) that extracts bits 7
    through 4 of the given integer a
    Example:
    extract(0x2020) returns 0b0010
*/

/*
#include <stdio.h>
#include <stdint.h>

// debug stuff for problem 3
int extract(int a){
  printf("a in binary: 0x%04X = 0b", a);

  for(int i = 15; i >= 0; i--) {
    printf("%d", (a >> i) & 1);
    if(i % 4 == 0) printf(" ");  // space every 4 bits
  }
  printf("\n");
  return (a >> 4) & 0xF;
}

int main() {
  uint8_t a = 15;
  // int a = 0x2020;
  int x = extract(a);
  printf("0b%04b\n", x);
  // print !a and ~a
  printf("!a = %d\n", !a);
  printf("~a = %u\n", (uint8_t)(~a));  // prints 240
  printf("~a = %u\n", a);  // prints 15
  return 0;
}
*/

int extract(int a){
  return (a >> 4) & 0xF;
}

int main() {
  uint8_t a = 15;
  // int a = 0x2020;
  int x = extract(a);
  printf("0b%04b\n", x);
  // print !a and ~a
  printf("!a = %d\n", !a);
  printf("~a = %u\n", (uint8_t)(~a));  // prints 240
  printf("~a = %u\n", a);  // prints 15
  return 0;
}
