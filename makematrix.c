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

main(argc,argv) 
int argc;
char *argv[];
{
int inc=50;
int maxcol=500;
int row=0,col=0,argind;
char *c;

   for (argind=1; argind<argc; argind++) {

      if (!strcmp(argv[argind],"-space")) {
              sscanf(argv[++argind],"%d",&inc);
      }
      else if (!strcmp(argv[argind],"-maxcol")) {
              sscanf(argv[++argind],"%d",&maxcol);
      }

   }

   while (gets(inpline)) {
 
      printf("#P %d %d\n",col,row);
 
      for (c=inpline; *c!=' ' && *c!='\0'; c++) {  
         if (*c=='!') printf(".\n");
         else printf("%c",*c); 
      }

      col+=inc; 
      if (col>=maxcol) {col=0; row+=inc;}

      printf("\n");

   }
}
