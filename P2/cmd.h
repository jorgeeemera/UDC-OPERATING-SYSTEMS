#ifndef CMD_H
#define CMD_H

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

void do_Allocate(char *tr[]);

void do_AllocateMalloc(char *tr[]);

void do_AllocateMmap(char *tr[]);

void do_AllocateShared(char *tr[]);

void do_AllocateCreateShared(char *tr[]);

void do_Deallocate(char *tr[]);

void do_DeallocateDelkey (char *tr[]);

void do_DeallocateMalloc (char *tr[]);

void Cmd_Memory(char *tr[]);

void recurse(char *tr[]);

void Cmd_ReadFile (char *tr[]);

void Cmd_WriteFile(char *tr[]);

void Cmd_MemFill(char *tr[]);

void Cmd_MemDump(char *tr[]);

void Cmd_Read(char *tr[]);

void Cmd_Write(char *tr[]);

void Cmd_GetUid(char *tr[]);

void Cmd_SetUid(char *tr[]);

void Cmd_ShowVar(char *tr[], char *envp[]);

void Cmd_ChangeVar(char *tr[], char *envp[]);

void Cmd_SubsVar(char *tr[], char *envp[]);

void Cmd_Environ(char *tr[], char *envp[]);

void Cmd_fork();

void Cmd_Search(char *tr[]);

void Cmd_Exec(char *tr[]);

void Cmd_ExecPri(char *tr[]);

int Cmd_fg(char *tr[]);

int Cmd_fgpri(char *tr[]);

void Cmd_Back(char *tr[]);

void Cmd_BackPri(char *tr[]);

void Cmd_Listjobs();

void Cmd_DelJobs(char *tr[]);

#endif //CMD_H
