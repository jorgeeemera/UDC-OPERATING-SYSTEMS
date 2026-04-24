#ifndef PROCESSCOMMAND_H
#define PROCESSCOMMAND_H

#include "includes.h"
#include "cmd.h"
#include "prompt.h"
#include "linkedlist.h"

#define MAXNAME 256

bool procesarEntrada(const char *buffer, char *trozos[], char *envp[]);

#endif //PROCESSCOMMAND_H
