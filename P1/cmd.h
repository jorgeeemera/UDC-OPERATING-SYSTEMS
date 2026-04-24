#ifndef P1_C_CMD_H
#define P1_C_CMD_H

#include "includes.h"

void cmdOpen (char * tr[]);

void cmdClose (char *tr[]);

void cmdDup (char * tr[]);

void cmdCd (char *tr[]);

void cmdAuthors(char *tr[]);

void getPid(char * trozos[]);

void getPPid(char * trozos[]);

void getDate(char *tr[]);

void cmdHistoric(char *tr[]);

void infoSys(char *tr[]);

void help(char *tr[]);

void makeFile(char *tr[]);

void makeDir(char *tr[]);

void listFile(char *tr[]);

void cwd(char *tr[]);

void listdir(char *tr[]);

void reclist(char *tr[]);

void revlist(char *tr[]);

void erase(char *tr[]);

void delrec(char *tr[]);

#endif //P1_C_CMD_H
