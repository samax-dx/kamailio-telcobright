#include <stdio.h>   /* printf */
#include <stdlib.h>  /* atoi, malloc */
#include <string.h>  /* strcpy */
#include "uthash.h"

struct Endpoint {
    char *ip;
    char *port;
};

struct NetworkLeg {
    struct Endpoint source, destination;
};

struct Call {
    char *call_id;
    struct NetworkLeg alice, bob;
};

struct CallWrapper {
    char *call_id;                    /* key */
    struct Call call;
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct CallWrapper *calls = NULL;

void add_user(char *call_id, struct Call call)
{
    struct CallWrapper *cw;

    HASH_FIND_STR(calls, call_id, cw);/* id already in the hash? */

    if (cw == NULL) {
        cw = (struct CallWrapper*)malloc(sizeof *cw);
        cw->call_id = call_id;
        cw->call = call;
        HASH_ADD_STR(calls, call_id, cw);  /* id is the key field */
    }
}

struct CallWrapper *find_user(char *call_id)
{
    struct CallWrapper *cw;
    HASH_FIND_STR(calls, call_id, cw);  /* s: output pointer */
    return cw;
}


int main()
{
    struct Call call;
    struct CallWrapper *cw;

    call.alice.source.ip = "123.321.456.987";
    call.alice.source.port = "8080";
    add_user("call1", call);
    cw = find_user("call1");
    printf("%s\n", cw->call.alice.source.ip);
    printf("%s\n", cw->call.alice.source.port);
    return 0;
}
