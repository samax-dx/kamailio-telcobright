//
// Created by mustafa on 7/28/22.
//

#ifndef KAMAILIO_VPP_H
#define KAMAILIO_VPP_H

struct Endpoint {
    char *ip;
    char *port;
};

struct NetworkLeg {
    struct Endpoint source, destination;
};

struct CnatRule {
    struct NetworkLeg match, target;
};

struct Call {
    char *call_id;
    struct NetworkLeg alice, bob;
};

void invite(char *call_id, char *old_ip, char *old_port, char *new_ip, char *new_port);
void ok200(char *call_id, char *old_ip, char *old_port, char *new_ip, char *new_port);
char *buildCnatCmd(struct Endpoint p_dst, struct Endpoint p_src, struct Endpoint dst);
void buildCnatRule(struct Call *call);
/*void print_calls();*/

#endif //KAMAILIO_VPP_H
