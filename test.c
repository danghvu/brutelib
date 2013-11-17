#include <stdio.h>
#include <string.h>
#include "brute.h"
#include <openssl/des.h>

// this file bruteforce to find a key such that DES("AAAAAAAA", key) == "\x2d\x91\x66\x72\xa9\x0c\xed\x48"

void checker(char pw[8], int len) {
  DES_key_schedule k;

  unsigned char out[8] = "";
  DES_set_key_unchecked((const_DES_cblock *)pw, &k);
  DES_ecb_encrypt((const_DES_cblock *)"AAAAAAAA", (DES_cblock *)out, &k, 1);
  if (!memcmp(out, "\x2d\x91\x66\x72\xa9\x0c\xed\x48", 8)){
    printf("pwned: %s\n", pw);
    fflush(stdout);
  }
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Usage: %s <number of processes>\n", argv[0]);
    return 1;
  }

  size_t nthread = atoi(argv[1]);

  bruteforce_set_debug(1);
  bruteforce_set_key("", 8);

  bruteforce_set_charset("1234", 4);
  bruteforce(nthread, (check_func_t)checker);

  bruteforce_set_charset("ABDFHJLNPRTVXZ", 14);
  bruteforce(nthread, (check_func_t)checker);

  bruteforce_set_charset("abdfhjlnprtvxz", 14);
  bruteforce(nthread, (check_func_t)checker);

  return 0;
}
