typedef void(*check_func_t)(char *, size_t);
void bruteforce_set_debug(int value);
void bruteforce_set_key(char * keystr, size_t max_keylen);
void bruteforce_set_charset(char * charset, size_t charset_len);
void bruteforce(size_t nthreads, check_func_t check);
