#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct hostent* gethost(char* argv) {
    int i;
    struct hostent* he;
    struct in_addr** addr_list;

    if ((he = gethostbyname(argv)) == NULL) {
        herror("gethostbyname");
        return 0;
    }

    //printf information about this host:
    printf("Offical name is: %s\n",he->h_name);
    printf("IP addresses:\n");
    addr_list = (struct in_addr** )he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        printf("%s\n",inet_ntoa(*addr_list[i]));
    }
    printf("\n");
    return he;
}

