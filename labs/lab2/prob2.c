#include <stdio.h>

int parity(int input) {
    int num_bits = sizeof(int) * 8;
    int result = 0;

    // XOR all bits together
    for (int i = 0; i < num_bits; i++) {
        result ^= (input >> i) & 1;
    }

    return result;
}


int main() {
  int x = parity(23);
  printf("Parity of 23 is %d\n", x);
  return 0;
}