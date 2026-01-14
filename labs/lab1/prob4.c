#define ARR_SIZE 5

typedef struct My_Struct {
  int i;
  char c;
} My_Struct;

int main() {
  My_Struct my_arr[ARR_SIZE];
  for(int i=0; i<ARR_SIZE; i++) {
    my_arr[i].i = (i << 4) * 3 - (2 << i);
      my_arr[i].c = my_arr[i].i;
  }

  my_arr[2].c = 'a';
  return 0;
}


