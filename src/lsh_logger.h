#ifndef lsh_logger_h
#define lsh_logger_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void logger_set_filename(const char *file_name);
void logger_log(const char *format, ...);

#ifdef LSH

FILE *file = NULL;
static char *filename = NULL;

void logger_set_filename(const char *file_name) {
  if (filename)
    free(filename);
  filename = malloc(1024);
  strcpy(filename, file_name);
  file = fopen(file_name, "w");
  if (!file)
    exit(1);
  printf("%s\n", filename);
}

// variadic ...
void logger_log(const char *format, ...) {
  va_list args;
  va_start(args, format);
    vfprintf(file, format, args);
  va_end(args);
  fflush(NULL);
}


#endif
#endif