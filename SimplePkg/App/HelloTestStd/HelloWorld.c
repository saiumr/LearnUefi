#include <Uefi.h>
#include <stdio.h>
//#include <Library/OrderedCollectionLib.h>


int main(int argc, char ** argv) {
  printf("Hello World!\n");
  FILE *fp = fopen("test.txt", "rb");
  if (fp == NULL) {
    printf("OMG\n");
  }
  else {
    printf("OHHHHH\n");
  }
  fclose(fp);

  return 0;
}