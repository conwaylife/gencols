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
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include "defs.h"

FILE *loadpat();

main(argc,argv)
int argc;
char *argv[];
{
int phase,phases=1,tcol,bound;
int tcol1=5,tcol2=5;
int resultafter=0,genresult=0;
int delpat1=0,delpat2=0,synch=1;
HashTable pat1,pat2,tmppat1,tmppat2,align,scratch,testpat1,testpat2,maskpat1,maskpat2;
int argind;
char *pat1file=NULL,*pat2file=NULL,*testpat1file=NULL,*testpat2file=NULL,*maskpat1file=NULL,*maskpat2file=NULL;
FILE *pat1fptr,*pat2fptr;
int pat1offset=0,pat2offset=0;

   for (argind=1; argind<argc; argind++) {
      if (!strcmp(argv[argind],"-pat")) {
              pat1file=argv[++argind];
              pat2file=argv[++argind];
      }
      else if (!strcmp(argv[argind],"-nph")) {
              sscanf(argv[++argind],"%d",&phases);
      }
      else if (!strcmp(argv[argind],"-tc")) {
              sscanf(argv[++argind],"%d",&tcol1);
              sscanf(argv[++argind],"%d",&tcol2);
      }
      else if (!strcmp(argv[argind],"-gen")) {
              sscanf(argv[++argind],"%d",&resultafter);
      }
      else if (!strcmp(argv[argind],"-del1")) {
              delpat1=1;
      }
      else if (!strcmp(argv[argind],"-del2")) {
              delpat2=1;
      }
      else if (!strcmp(argv[argind],"-nosynch")) {
              synch=0;
      }
      else if (!strcmp(argv[argind],"-genafter")) {
              sscanf(argv[++argind],"%d",&genresult);
      }
      else if (!strcmp(argv[argind],"-test1")) {
              testpat1file=argv[++argind];
      }
      else if (!strcmp(argv[argind],"-test2")) {
              testpat2file=argv[++argind];
      }
      else if (!strcmp(argv[argind],"-mask1")) {
              maskpat1file=argv[++argind];
      }
      else if (!strcmp(argv[argind],"-mask2")) {
              maskpat2file=argv[++argind];
      }
      else if (!strcmp(argv[argind],"-leq")) {
              sscanf(argv[++argind],"%d",&bound);
              setupper(bound);
      }
      else if (!strcmp(argv[argind],"-geq")) {
              sscanf(argv[++argind],"%d",&bound);
              setlower(bound);
      }
      else if (!strcmp(argv[argind],"-filt")) {
              setfilters(argv[++argind]);
      }
      else {
         fprintf(stderr,"Unknown option: \"%s\"\n",argv[argind]);
         exit(0);
      }
   }

   if (testpat1file && maskpat1file) {
      fprintf(stderr, "Conflicting options: \"-test1\" \"-mask1\"");
      exit(0);
   }

   if (testpat2file && maskpat2file) {
      fprintf(stderr, "Conflicting options: \"-test2\" \"-mask2\"");
      exit(0);
   }

   allochash(&pat1,DEFAULTHSIZE);
   allochash(&pat2,DEFAULTHSIZE);
   allochash(&tmppat1,DEFAULTHSIZE);
   allochash(&tmppat2,DEFAULTHSIZE);
   allochash(&align,DEFAULTHSIZE);
   allochash(&scratch,DEFAULTHSIZE);

   if (testpat1file || maskpat1file) allochash(&testpat1,DEFAULTHSIZE);
   else testpat1.table=NULL;

   if (testpat2file || maskpat2file) allochash(&testpat2,DEFAULTHSIZE);
   else testpat2.table=NULL;

   if (maskpat1file) allochash(&maskpat1,DEFAULTHSIZE);
   else maskpat1.table=NULL;

   if (maskpat2file) allochash(&maskpat2,DEFAULTHSIZE);
   else maskpat2.table=NULL;

   pat1fptr=pat2fptr=NULL;

   while (pat1fptr=loadpat(pat1,NULL,1,pat1file,pat1fptr)) {
     pat1offset++;
     pat2offset = 0;
     while (pat2fptr=loadpat(pat2,NULL,1,pat2file,pat2fptr)) {
       pat2offset++;
       fprintf(stderr, "pat1: %u pat2: %u      \r", pat1offset, pat2offset);

       if (testpat1file) fclose(loadpat(testpat1,NULL,1,testpat1file,NULL));
       if (testpat2file) fclose(loadpat(testpat2,NULL,1,testpat2file,NULL));

       if (maskpat1file) fclose(loadpat(testpat1,&maskpat1,1,maskpat1file,NULL));
       if (maskpat2file) fclose(loadpat(testpat2,&maskpat2,1,maskpat2file,NULL));

       for (phase=1; phase<=phases; phase++) {
	 copypattern(tmppat1,pat1,0,0,NOFORCE);
	 copypattern(tmppat2,pat2,0,0,1);
	 delgen(align,SPECGEN);
	 for (tcol=1; tcol<tcol2; tcol++) {
	   interact(tmppat1,tmppat2,align,tcol);
	   gennosave(tmppat1,tcol);
	   gennosave(tmppat2,tcol);
	 }
	 interact(tmppat1,tmppat2,align,tcol);
	 for (tcol=tcol1; tcol<=tcol2; tcol++) {
	   if (resultafter) {
	     int i;
	     delgen(tmppat1,ALLGENS);
	     delgen(tmppat2,ALLGENS);
	     copypattern(tmppat1,pat1,0,0,1);
	     copypattern(tmppat2,pat2,0,0,1);
	     for (i=1; i<resultafter; i++) {
	       gennosave(tmppat1,i);
	       gennosave(tmppat2,i);
	     }
	   }
	   outcollisions(pat1,pat2,align,tcol,scratch,
			 resultafter,tmppat1,tmppat2,delpat1,delpat2,synch,
			 genresult,testpat1,testpat2,maskpat1,maskpat2,pat1offset,pat2offset,phase);
	 }
	 delgen(tmppat1,ALLGENS);
	 delgen(tmppat2,ALLGENS);
	 gennosave(pat2,phase);
	 if (testpat2.table) gennosave(testpat2,phase);
       }
     }
   }

   if (pat2offset) {
     fprintf(stderr, "\n");
   }
}
