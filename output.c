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
#include "defs.h"

char filterstring[MAXLINE]="apnf";
int maxcells=MAXPATLEN,mincells= -1;

void outputpattern(char *patstr1,char *patstr2,int numcells,int removefailed,int per,int xshift,int yshift,int pat1offset,int pat2offset,int phase);

outcollisions(pat1,pat2,align,gen,tmp,resultafter,
                       pat1gen,pat2gen,delpat1,delpat2,synch,genresult,
                       testpat1,testpat2,maskpat1,maskpat2,pat1offset,pat2offset,phase)
HashTable pat1,pat2,align,tmp,pat1gen,pat2gen,testpat1,testpat2,maskpat1,maskpat2;
int gen,resultafter,delpat1,delpat2,synch,genresult,pat1offset,pat2offset,phase;
{
  CellList **cell,**tcell;
  static char outpatini[MAXPATLEN],outpatfin[MAXPATLEN];
  int tgen,removefailed,count;
  HashTable remove1,remove2;

  remove1=(synch)?pat1gen:pat1;
  remove2=(synch)?pat2gen:pat2;

  for (cell=(CellList **)align.table;
       cell<(CellList **)align.table+align.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==SPECGEN && (*tcell)->u.val==gen) {
	delgen(tmp,ALLGENS);

        /* set up collision */
        copypattern(tmp,pat1,0,0,1);
        copypattern(tmp,pat2,-(*tcell)->x,-(*tcell)->y,1);

        /* copy picture of collision to a string */
        patstring(tmp,1,outpatini);

        if (!resultafter) printf("%s\n",outpatini);
        else {
	  int xshift,yshift,per;

#ifdef USEBOXES
	  genlist(tmp,1,resultafter-1);
	  tgen=resultafter;
#else
	  for (tgen=1; tgen<resultafter; tgen++) {
	    gennosave(tmp,tgen);
	  }
#endif

#define MAXOSCTEST 30
	  removefailed=0;
	  if (countpat(tmp,tgen)
	      ==countpat(pat1gen,tgen)+countpat(pat2gen,tgen)
	      && subpattern(pat1gen,tmp,0,0,tgen,tgen,&count,0)
	      && subpattern(pat2gen,tmp,-(*tcell)->x,-(*tcell)->y,tgen,tgen,&count,0))
	    per= -1;
	  else {
	    if (delpat1) {
	      if (subpattern(remove1,tmp,0,0,ALLGENS,tgen,&count,0))
		delpattern(remove1,tmp,0,0,ALLGENS,tgen);
	      else removefailed=1;
	    }
	    if (delpat2) {
	      if  (subpattern(remove2,tmp,
			      -(*tcell)->x,-(*tcell)->y,ALLGENS,tgen,&count,0))
		delpattern(remove2,tmp,
			   -(*tcell)->x,-(*tcell)->y,ALLGENS,tgen);
	      else removefailed=1;
	    }
	    if (testpat1.table) {
	      if (subpattern(testpat1,tmp,0,0,ALLGENS,tgen,&count,0)
                  && (!maskpat1.table || subpattern(maskpat1,tmp,0,0,ALLGENS,tgen,&count,1)))
		delpattern(testpat1,tmp,0,0,ALLGENS,tgen);
	      else removefailed=1;
	    }
	    if (testpat2.table) {
	      if  (subpattern(testpat2,tmp, -(*tcell)->x,-(*tcell)->y,ALLGENS,tgen,&count,0)
                   && (!maskpat2.table || subpattern(maskpat2,tmp,-(*tcell)->x,-(*tcell)->y,ALLGENS,tgen,&count,1)))
		delpattern(testpat2,tmp,
			   -(*tcell)->x,-(*tcell)->y,ALLGENS,tgen);
	      else removefailed=1;
	    }

#ifdef USEBOXES
	    genlist(tmp,tgen,genresult);
	    tgen+=genresult;
#else
	    for ( ; tgen<resultafter+genresult; tgen++) {
	      gennosave(tmp,tgen);
	    }
#endif

	    for (per=1; per<=MAXOSCTEST; per++) {
	      gensparse(tmp,tgen+per-1);
	      if (comparegen(tmp,tgen,tgen+per,&xshift,&yshift)) break;
	    }
	  }

	  if (1 || per>=1 && per<=MAXOSCTEST) patstring(tmp,tgen,outpatfin);
	  else strcpy(outpatfin,"not_shown");

	  outputpattern(outpatini,outpatfin,
			countpat(tmp,tgen),removefailed,
                        per,xshift,yshift,pat1offset,pat2offset,phase);

	}
      }
      tcell= &((*tcell)->next);
    }
  }
}

setfilters(fstring)
char *fstring;
{
   strcpy(filterstring,fstring);
}

setlower(lbound)
int lbound;
{
   mincells=lbound;
}

setupper(ubound)
int ubound;
{
   maxcells=ubound;
}

void outputpattern(patstr1,patstr2,numcells,removefailed,per,xshift,yshift,pat1offset,pat2offset,phase)
char *patstr1,*patstr2;
int per,numcells,removefailed,xshift,yshift;
{
int maxmove=0;

      if (xshift>maxmove) maxmove=xshift;
      if (-xshift>maxmove) maxmove= -xshift;
      if (yshift>maxmove) maxmove=yshift;
      if (-yshift>maxmove) maxmove= -yshift;

            if (numcells<mincells || numcells>maxcells) return;
            if (removefailed && !strchr(filterstring,'f') ) return;
            if (per== -1 && !strchr(filterstring,'n') ) return;
            if (per>MAXOSCTEST && !strchr(filterstring,'a') ) return;
            if (per>=1 && per <= MAXOSCTEST && !strchr(filterstring,'p') &&
                !strchr(filterstring,'0'+per)
                && (maxmove!=1 || !strchr(filterstring,'g'))
                && (maxmove!=2 || !strchr(filterstring,'s'))) return;

            printf("%s %s %d",patstr1,patstr2,numcells);

            if (removefailed) printf(" remove_failed");
            else if (per== -1) printf(" none");
            else if (per>MAXOSCTEST) printf(" other");
            else printf(" (%d,%d,%d)",per,xshift,yshift);

            printf(" [%u,%u,%u]", pat1offset, pat2offset, phase);

            printf("\n");

            fflush(stdout);
}
