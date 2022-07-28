#include <string.h>
#include <stdlib.h>
#include "vpp.h"
#include "uthash.h"
#include <stdio.h>

// struct Call call;

struct CallWrapper {
    char *call_id;
    struct Call call;
    UT_hash_handle hh;
};

struct CallWrapper *calls = NULL;

struct Call * get_call_from_ht(char *call_id)
{
    struct CallWrapper *cw;
    HASH_FIND_STR(calls, call_id, cw);
    if(cw == NULL) {
        cw = (struct CallWrapper*)malloc(sizeof *cw);
        cw->call_id = call_id;
        HASH_ADD_STR(calls, call_id, cw);
        /*unsigned int num_calls;
        num_calls = HASH_COUNT(calls);
        if(num_calls==2){
            print_calls();
        }*/
    }
    struct Call *call;
    call = &cw->call;

    return call;
}


/*void print_calls()
{
    struct CallWrapper *cw;
    FILE *callsData;
    callsData=fopen("call-log.txt","a");
    for (cw = calls; cw != NULL; cw = (struct CallWrapper*)(cw->hh.next)) {
        //printf("user id %d: name %s\n", cw->id, cw->name);
        fprintf(callsData, "\n call id -> %s\t alice port -> %s \t bob port-> %s \n", cw->call_id,cw->call.alice.source.port,cw->call.bob.source.port);

    }
    fclose(callsData);
}*/



void invite(char *call_id, char *old_ip, char *old_port, char *new_ip, char *new_port) {
    struct Call *call;
    call = get_call_from_ht(call_id);
    call->call_id = (char*)malloc(strlen(call_id)+1);
    call->alice.source.ip = (char*)malloc(strlen(old_ip)+1);
    call->alice.source.port = (char*)malloc(strlen(old_port)+1);
    call->bob.source.ip = (char*)malloc(strlen(new_ip)+1);
    call->bob.source.port = (char*)malloc(strlen(new_port)+1);
    /* copy */
    strcpy(call->call_id,call_id);
    strcpy(call->alice.source.ip, old_ip);
    strcpy(call->alice.source.port, old_port);
    strcpy(call->bob.source.ip, new_ip);
    strcpy(call->bob.source.port, new_port);
    /* separate */
    call->alice.source.ip = strtok(call->alice.source.ip, "\r\n");
    call->alice.source.port = strtok(call->alice.source.port, "RTP/");

}

void ok200(char *call_id, char *old_ip, char *old_port, char *new_ip, char *new_port) {
    struct Call *call;
    call = get_call_from_ht(call_id);
    call->alice.destination.ip = (char*)malloc(strlen(new_ip)+1);
    call->alice.destination.port = (char*)malloc(strlen(new_port)+1);
    call->bob.destination.ip = (char*)malloc(strlen(old_ip)+1);
    call->bob.destination.port = (char*)malloc(strlen(old_port)+1);
    strcpy(call->alice.destination.ip, new_ip);
    strcpy(call->alice.destination.port, new_port);
    strcpy(call->bob.destination.ip, old_ip);
    strcpy(call->bob.destination.port, old_port);
    call->bob.destination.ip = strtok(call->bob.destination.ip, "\r\n");
    call->bob.destination.port = strtok(call->bob.destination.port, "RTP/");

    buildCnatRule(call);
}

char *buildCnatCmd(struct Endpoint p_dst, struct Endpoint p_src, struct Endpoint dst) {
    char *prefix = "CNAT=\"cnat translation add proto UDP vip";
    char *postfix = " \" && sudo vppctl $CNAT";
    char *cmd=(char*) malloc(200);
    cmd = strcpy(cmd, prefix);
    strcat(cmd, " ");
    strcat(cmd, p_dst.ip);
    strcat(cmd, " ");
    strcat(cmd, p_dst.port);
    strcat(cmd, " to ");
    strcat(cmd, p_src.ip);
    strcat(cmd, " ");
    strcat(cmd, p_src.port);
    strcat(cmd, "->");
    strcat(cmd, dst.ip);
    strcat(cmd, " ");
    strcat(cmd, dst.port);
    strcat(cmd, postfix);
    strcat(cmd, "\0");
    return cmd;
}

void buildCnatRule(struct Call *call) {
    struct CnatRule aliceCnat;
    struct CnatRule bobCnat;
/* alice cnat*/
    aliceCnat.match.source.ip =call->alice.source.ip;
    aliceCnat.match.source.port = call->alice.source.port;
    aliceCnat.match.destination.ip = call->alice.destination.ip;
    aliceCnat.match.destination.port = call->alice.destination.port;
    aliceCnat.target.source.ip = call->bob.source.ip;
    aliceCnat.target.source.port = call->bob.source.port;
    aliceCnat.target.destination.ip = call->bob.destination.ip;
    aliceCnat.target.destination.port = call->bob.destination.port;
        /* bob cnat*/

    bobCnat.match.source.ip = call->bob.destination.ip;
    bobCnat.match.source.port = call->bob.destination.port;
    bobCnat.match.destination.ip = call->bob.source.ip;
    bobCnat.match.destination.port = call->bob.source.port;

    bobCnat.target.source.ip = call->alice.destination.ip;
    bobCnat.target.source.port = call->alice.destination.port;
    bobCnat.target.destination.ip = call->alice.source.ip;
    bobCnat.target.destination.port = call->alice.source.port;

    char *aliceCmd = buildCnatCmd(aliceCnat.match.destination, aliceCnat.target.source, aliceCnat.target.destination);
    system(aliceCmd);
    char *bobCmd = buildCnatCmd(bobCnat.match.destination, bobCnat.target.source, bobCnat.target.destination);
    system(bobCmd);
}
