#ifndef INC_68K_SRC_STDSTREAMS_H
#define INC_68K_SRC_STDSTREAMS_H

size_t stdout_write(FILE* instance, const char* bp, size_t n);
size_t stdin_read(FILE* instance, char* bp, size_t n);

#endif
