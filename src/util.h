#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifdef WIN32
#ifndef strcasecmp
# define strcasecmp stricmp
# define strncasecmp strnicmp
#endif
# define mkdir(path, mode) mkdir (path)
#endif

void strtolower (char *string);

const char *extract_path_segment (const char *path,
				  char *buffer, size_t size);

void *xmalloc (size_t size);
void *xcalloc (size_t size);
void xfree (void *ptr);
char *xstrdup (const char *str);

#endif
