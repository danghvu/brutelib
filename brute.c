#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int wait(int *);

typedef void(*check_func_t)(char *, size_t);

void bruteforce_set_debug(int value);
void bruteforce_set_key(char * keystr, size_t max_keylen);
void bruteforce_set_charset(char * charset, size_t charset_len);
void bruteforce(size_t nthreads, check_func_t check);


#define MAX_BRUTE 512

static size_t do_fork();
static void do_bruteforce();
static void wait_for_finish();

// ----------------------------------------------------------------------------
// VARIABLES
// ----------------------------------------------------------------------------
unsigned char charset[MAX_BRUTE];
size_t charset_len;

char key[MAX_BRUTE];
int pos[MAX_BRUTE];
size_t key_len = 0;
size_t max_keylen;

check_func_t check_func;

size_t nthreads;
size_t thread_index = 0;

int debug = 1;

// ----------------------------------------------------------------------------
// FUNCTIONS
// ----------------------------------------------------------------------------
void bruteforce_set_debug(int value) {
  debug = value;
}

void bruteforce_set_charset(char * _charset, size_t _charset_len) {
  charset_len = _charset_len;
  if (charset_len > sizeof(charset)) {
    printf("ERROR: too long charset (%zu bytes)\n", charset_len);
    exit(100);
  }
  memcpy(charset, _charset, charset_len);
}

void bruteforce_set_key(char * keystr, size_t _max_keylen) {
  max_keylen = _max_keylen;
  thread_index = 0;
  key_len = 0;
  bzero(key, sizeof(key));
  bzero(pos, sizeof(pos));
  if (strlen(keystr) > 0) {
    // not implemented
  }
}

void bruteforce(size_t _nthreads, check_func_t _check_func) {
  nthreads = _nthreads;
  check_func = _check_func;

  if (debug) printf("Starting bruteforce with %zu threads...\n", nthreads);
  if (debug) printf("Charset: '%s' (%zu bytes)\n", charset, charset_len);
  if (debug) printf("Key: up to %zu bytes\n", max_keylen);
  if (debug) printf("\n");

  if ( do_fork() ) {
    do_bruteforce();
    exit(0);
  }

  wait_for_finish();
  printf("Bruteforce ended\n");

  return;
}

static size_t do_fork() {
  size_t forks_left = nthreads;
  while (forks_left) {
    if (fork()) {
      forks_left -= 1;
    }
    else {
      break;
    }
    thread_index++;
  }
  return forks_left;
}

static void wait_for_finish() {
  int status;
  size_t i;
  for (i = 0; i < nthreads; i++) {
    wait(&status);
  }
}

static void do_bruteforce() {
  while (1) {
    check_func(key, key_len);

    int add = 1, nesting = 0;
    while (add) {
      if (nesting >= key_len) { // key growing
        key_len = nesting + 1;
        pos[nesting] = (nesting == 0)?thread_index:0;
        if (debug) printf("Raised key length up to %zu...\n", key_len);
      }
      else { // increasing char
        pos[nesting] += (nesting == 0)?nthreads:1;
      }

      // check for overflow
      if (pos[nesting] >= charset_len) {
        pos[nesting] = (nesting == 0)?thread_index:0;
      } else {
        add = 0;
      }
      // save increased char
      key[nesting] = charset[pos[nesting]];
      nesting++;

      // check for key length limit
      if (add && nesting > max_keylen - 1) {
        return;
      }
    }
  }
  return;
}
