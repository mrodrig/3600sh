#include "shell_cmds.h"

#define MAX_HOSTNAME_LENGTH 256

// Retrieve the hostname of the system
char *retrieve_hostname() {
  char hostname[MAX_HOSTNAME_LENGTH]; // FQDN max length is 255 characters; +1 for null byte terminator
  gethostname(hostname, MAX_HOSTNAME_LENGTH); // use built-in function to get the hostname
  char *host = (char *) calloc(strlen(hostname)+1, sizeof(char)); // allocate a string to store it for returning
  if (host == NULL) { // error during allocation
    printf("Error: could not allocate memory for hostname.\n");
    exit(1);
  }
  sprintf(host, "%s", hostname); // copy the hostname into host
  host[strlen(hostname)] = '\0'; // null-terminate the host variable
  return host; // return a pointer to the host variable
}
