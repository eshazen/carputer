
#include <string.h>
#include <ctype.h>

// #define TEST_MAIN

#ifdef TEST_MAIN

#include <stdio.h>
void replac( char *str, char *srch, char *repl);

int main( int argc, char *argv[]) {
  if( argc < 4) {
    fprintf( stderr, "usage: %s \"<original-string>\" <search> <replace>\n", argv[0]);
    return 1;
  }

  if( strlen( argv[3]) > strlen( argv[2])) {
    fprintf( stderr, "Replacement must be shorter than search string\n");
    return 1;
  }

  replac( argv[1], argv[2], argv[3]);
  printf( "\"%s\"\n", argv[1]);
	  
}

#endif

//
// ARM arduino libc doesn't have strcasestr
// so use this AI-generated one
//
char *my_strcasestr(const char *haystack, const char *needle) {
    char c, sc;
    size_t len;

    // Handle the edge case of an empty needle string, which is a substring of everything
    if ((c = *needle++) == '\0') {
        return (char *)haystack;
    }
    // Convert the first character of needle to lowercase once
    c = tolower((unsigned char)c);
    len = strlen(needle);

    do {
        // Find the first occurrence of the first character of needle in haystack, ignoring case
        do {
            if ((sc = *haystack++) == '\0') {
                return NULL; // Reached end of haystack, no match found
            }
        } while ((char)tolower((unsigned char)sc) != c);
        
        // Check the rest of the needle against the rest of the haystack
    } while (strncasecmp(haystack, needle, len) != 0); // strncasecmp is also a non-standard function

    // If the loop finishes, a match was found.
    // Return a pointer to the start of the match in the original haystack string
    return (char *)(haystack - 1);
}

//
// search for <srch> in <str>, replace with <repl>
// <repl> must be shorter than <srch>
// NOTE:  modifies <str>
//
void replac( char *str, const char *srch, const char *repl) {
  char *p = my_strcasestr( str, srch);
  if( p) {
    // copy over replace string
    for( int i=0; i<strlen( repl); i++)
      p[i] = repl[i];
    // move tail
    memmove( p+strlen(repl), p+strlen(srch), strlen(str)-strlen(p+strlen(srch)));
  }
}
