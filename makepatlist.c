/*
 * Gencols: Copyright 1994, Paul Callahan, callahan@cs.jhu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 */


#include <stdio.h>
#include <limits.h>
#include <malloc.h>
#include "defs.h"

char inpline[MAXPATLEN];
FILE *loadpat();

main(argc,argv) 
int argc;
char *argv[];
{
int argind;
HashTable pat;
char patstr[MAXPATLEN];

   allochash(&pat,DEFAULTHSIZE);

   for (argind=1; argind<argc; argind++) {
       fclose(loadpat(pat,NULL,1,argv[argind],NULL));
       printf("%s\n",patstring(pat,1,patstr));
   }
}
