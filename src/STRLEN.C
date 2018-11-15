/*
 * Here I experimented by trying to create a suposedly more efficient strlen()
 * function in assembly. While my asm version is certainly faster, the
 * _asm overhead makes it also a lot bigger.
 *
 * how to build:
 * wcl -y -0 -d0 -lr -ms -we -wx -fm=strlen.map -os strlen.c -fe=strlen.exe
 */


#include <stdio.h>
#include <i86.h>


unsigned short mystrlen(void far *s) {
  unsigned short res = 0;
  while (*(unsigned char far *)s != 0) {
    res++;
    s = ((unsigned char far *)s) + 1;
  }
  return(res);
}


unsigned short mystrlenasm(void far *s) {
  unsigned short res = 0, sseg, soff;
  sseg = FP_SEG(s);
  soff = FP_OFF(s);
  _asm {
    xor al, al
    mov es, sseg
    mov di, soff
    mov cx, 0ffffh
    cld    /* clear the direction flag so repne proceeds forward */
    repne scasb
    inc cx
    not cx
    mov res, cx
  }
  return(res);
}


int main(void) {
  char *s[] = {"Mateusz", "", "123", "Hello, World!", NULL};
  int i;
  int r1, r2;

  for (i = 0; s[i] != NULL; i++) {
    r1 = mystrlen(s[i]);
    r2 = mystrlenasm(s[i]);

    printf("'%s' [r1] = %d\n'%s' [r2] = %d\n", s[i], r1, s[i], r2);
  }
  return(0);
}
