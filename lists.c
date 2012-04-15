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

#define HASHMULT 1999
#define hashtriple(x,y,gen,hsize) mod(((((x*HASHMULT)+y)*HASHMULT)+gen),hsize)

/*
#define TRACEHASH 1
*/

#ifdef TRACEHASH
static int lookups=0, tries=0;
#endif

CellList **findcell(x,y,gen,hash)
int x,y,gen;
HashTable hash;
{
  CellList **cell;

#ifdef TRACEHASH
  lookups++;
#endif


  for (cell=(CellList **)hash.table+hashtriple(x,y,gen,hash.hsize); 
                                              *cell; cell= &((*cell)->next)) {
#ifdef TRACEHASH
  tries++;
#endif
    if (x==(*cell)->x && y==(*cell)->y && gen==(*cell)->gen) break;
  }

  return cell;
}

void allochash(hash,n)
HashTable *hash;
int n;
{
void **hptr;

   hash->table=(void **)malloc(n*sizeof(void *));
   hash->hsize=n;
   for (hptr=hash->table; hptr<hash->table+hash->hsize; hptr++) *hptr=NULL;
}


void setcell(x,y,gen,val,hash)
int x,y,gen,val;
HashTable hash;
{
 
  CellList **cell;
  
  cell=findcell(x,y,gen,hash);

  if (!(*cell)) {
     (*cell)=(CellList *)malloc(sizeof(CellList));
     (*cell)->x=x;
     (*cell)->y=y;
     (*cell)->gen=gen;
     (*cell)->next=NULL;
   }

  (*cell)->u.val=val;
}

int inccell(x,y,gen,hash)
int x,y,gen;
HashTable hash;
{

  CellList **cell;
  
  cell=findcell(x,y,gen,hash);

  if (!(*cell)) {
    (*cell)=(CellList *)malloc(sizeof(CellList));
    (*cell)->x=x;
    (*cell)->y=y;
    (*cell)->gen=gen;
    (*cell)->next=NULL;
    (*cell)->u.val=1;
  }
  else  (*cell)->u.val++;

  return (*cell)->u.val;

}


void deletecell(x,y,gen,hash)
int x,y,gen;
HashTable hash;
{
  CellList **cell,*tofree;

  cell=findcell(x,y,gen,hash);

  if (*cell) dodelete(cell);

}

void freehash(hash) 
HashTable *hash;
{
  CellList **cell,**tcell;

  for (cell=(CellList **)hash->table; 
       cell<(CellList **)hash->table+hash->hsize; cell++) {
    tcell=cell;
    while (*tcell) dodelete(tcell)
    }

  free(hash->table);
}

int getcell(x,y,gen,hash)
int x,y,gen;
HashTable hash;
{
  CellList **cell;

  cell=findcell(x,y,gen,hash);
  if (*cell) return (*cell)->u.val;
  else return 0;

}

int isglider(x,y,dx,dy,gen,hash) 
int x,y,gen,dx,dy;
HashTable hash;
{
   return 
       !getcell(x-2*dx,y-2*dy,gen,hash) &&
       !getcell(x-2*dx,y-1*dy,gen,hash) &&
       !getcell(x-2*dx,y-0*dy,gen,hash) &&
       !getcell(x-2*dx,y+1*dy,gen,hash) &&
       !getcell(x-2*dx,y+2*dy,gen,hash) &&

       !getcell(x-1*dx,y-2*dy,gen,hash) &&
       !getcell(x-1*dx,y-1*dy,gen,hash) &&
        getcell(x-1*dx,y-0*dy,gen,hash) &&
       !getcell(x-1*dx,y+1*dy,gen,hash) &&
       !getcell(x-1*dx,y+2*dy,gen,hash) &&

       !getcell(x-0*dx,y-2*dy,gen,hash) &&
       !getcell(x-0*dx,y-1*dy,gen,hash) &&
       !getcell(x-0*dx,y-0*dy,gen,hash) &&
        getcell(x-0*dx,y+1*dy,gen,hash) &&
       !getcell(x-0*dx,y+2*dy,gen,hash) &&

       !getcell(x+1*dx,y-2*dy,gen,hash) &&
        getcell(x+1*dx,y-1*dy,gen,hash) &&
        getcell(x+1*dx,y-0*dy,gen,hash) &&
        getcell(x+1*dx,y+1*dy,gen,hash) &&
       !getcell(x+1*dx,y+2*dy,gen,hash) &&

       !getcell(x+2*dx,y-2*dy,gen,hash) &&
       !getcell(x+2*dx,y-1*dy,gen,hash) &&
       !getcell(x+2*dx,y-0*dy,gen,hash) &&
       !getcell(x+2*dx,y+1*dy,gen,hash) &&
       !getcell(x+2*dx,y+2*dy,gen,hash);
}

reportallgliders(gen,hash)
int gen;
HashTable hash;
{
   int dx,dy,curgen;
   CellList **cell,**tcell;

   for (curgen=gen; curgen<gen+4; curgen++) {
      for (dx=-1; dx<=1; dx+=2)
         for (dy=-1; dy<=1; dy+=2) {
           for (cell=(CellList **)hash.table; 
              cell<(CellList **)hash.table+hash.hsize; cell++) {
              tcell=cell;
              while (*tcell) {
                 if ((*tcell)->gen==curgen) {
                   if (isglider((*tcell)->x+dx,(*tcell)->y,dx,dy,curgen,hash))
                     printf("%5d %5d %5d %5d %5d\n",
                             (*tcell)->x+dx,(*tcell)->y,dx,dy,curgen);   
                 }
                 tcell= &((*tcell)->next);
             } 
           }
         }
      gennosave(hash,curgen);
   }
}


static int rules[][9]={{0,0,0,1,0,0,0,0,0},{0,0,1,1,0,0,0,0,0}};

gensparse(hash,gen)
int gen;
HashTable hash;
{
  CellList **cell,**tcell;
  int i;

  /* assumes next generation has all zero entries */

  /* calculate neighborhoods */
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen) {
	inccell((*tcell)->x-1,(*tcell)->y-1,gen+1,hash);
	inccell((*tcell)->x  ,(*tcell)->y-1,gen+1,hash);
	inccell((*tcell)->x+1,(*tcell)->y-1,gen+1,hash);
	inccell((*tcell)->x-1,(*tcell)->y  ,gen+1,hash);
	inccell((*tcell)->x+1,(*tcell)->y  ,gen+1,hash);
	inccell((*tcell)->x-1,(*tcell)->y+1,gen+1,hash);
	inccell((*tcell)->x  ,(*tcell)->y+1,gen+1,hash);
	inccell((*tcell)->x+1,(*tcell)->y+1,gen+1,hash);

      }
      tcell= &((*tcell)->next);
    }
  }


  /* find next generation */
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen+1) {
	if (rules[getcell((*tcell)->x,(*tcell)->y,gen,hash)][(*tcell)->u.val]) {
	  (*tcell)->u.val=1;
          tcell= &((*tcell)->next);
        }
	else
	  dodelete(tcell);
      } else tcell= &((*tcell)->next);
    }
  }
}

gennosave(hash,gen)
int gen;
HashTable hash;
{
  gensparse(hash,gen);
  delgen(hash,gen);
}


outhash(hash) 
HashTable hash;
{
int i;
CellList *cell;

   for (i=0; i<hash.hsize; i++) {
     if (cell=(CellList *)hash.table[i]) {
       printf("Table entry %d\n",i);
       while (cell) {
         printf("x=%d y=%d gen=%d val=%d\n",
                 cell->x,cell->y,cell->gen,cell->u.val);
         cell=cell->next;
       }
       printf("\n");
     }
   }

}

outgen(hash,gen) 
HashTable hash;
int gen;
{
int i;
CellList *cell;

   for (i=0; i<hash.hsize; i++) {
     if (cell=(CellList *)hash.table[i]) {
       while (cell) {
         if ((cell->gen==gen)&&(cell->u.val)) 
             printf("%d %d\n",cell->x,cell->y);
         cell=cell->next;
       }
     }
   }

}

delgen(hash,gen) 
HashTable hash;
int gen;
{
  int i;
  CellList **cell,**tcell;

  for (cell=(CellList **)hash.table; 
          cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if (gen==ALLGENS || ((*tcell)->gen==gen)) dodelete(tcell)
      else tcell= &((*tcell)->next);
    }
  }
}

copypattern(topat,frompat,dx,dy,forcegen)
HashTable topat,frompat;
int dx,dy,forcegen;
{
  CellList **cell,**tcell;

  for (cell=(CellList **)frompat.table; 
       cell<(CellList **)frompat.table+frompat.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      setcell(((*tcell)->x)+dx,((*tcell)->y)+dy,
	      (forcegen==NOFORCE)?(*tcell)->gen:forcegen, 
                               ((*tcell)->u.val),topat);
    tcell= &((*tcell)->next);
  }
}
}

boundingrect(hash,gen,x1,y1,x2,y2)
HashTable hash;
int gen;
int *x1,*y1,*x2,*y2;
{
  CellList **cell,**tcell;

  *x1= *y1=INT_MAX;
  *x2= *y2=INT_MIN;
 
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen) {
	if ((*tcell)->x < *x1) *x1=((*tcell)->x);
	if ((*tcell)->y < *y1) *y1=((*tcell)->y);
	if ((*tcell)->x > *x2) *x2=((*tcell)->x);
	if ((*tcell)->y > *y2) *y2=((*tcell)->y);
      }
      tcell= &((*tcell)->next);
    }
  }
}

int countpat(hash,gen)
HashTable hash;
int gen;
{
  CellList **cell,**tcell;
  int cellcount=0;
 
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen) {
         if ((*tcell)->u.val) cellcount++;
      }
      tcell= &((*tcell)->next);
    }
  }

  return cellcount;
}

printpic(hash,gen) 
HashTable hash;
int gen;
{
  int x1,y1,x2,y2;
  int x,y,val;
  static char outstr[]=".*23456789abcdef";

  boundingrect(hash,gen,&x1,&y1,&x2,&y2);

  for (y=y1; y<=y2; y++) { 
    for (x=x1; x<=x2; x++) {
      val=getcell(x,y,gen,hash);
      putchar(val<16?outstr[val]:'+');
    }
    printf("\n");
  }

}

int match(pat1,pat2,align,gen1,gen2,sx,sy,transpose,mtx,mty)
HashTable pat1,pat2,align;
int gen1,gen2,sx,sy,transpose;
int *mtx,*mty;
{
  CellList **cell1,**tcell1,**cell2,**tcell2;
  int x,y,tmp;
  int maxmatch=0,countmatch;

  for (cell1=(CellList **)pat1.table; 
       cell1<(CellList **)pat1.table+pat1.hsize; cell1++) {
    tcell1=cell1;
    while (*tcell1) {
      if ((*tcell1)->gen==gen1) {

        x=sx*(*tcell1)->x;
        y=sy*(*tcell1)->y;
        if (transpose) {tmp=x; x=y; y=tmp;}

	for (cell2=(CellList **)pat2.table; 
	     cell2<(CellList **)pat2.table+pat2.hsize; cell2++) {
	  tcell2=cell2;
	  while (*tcell2) {
	    if ((*tcell2)->gen==gen2) {
              countmatch=inccell((*tcell2)->x-x,
                                 (*tcell2)->y-y,SPECGEN,align);
              if (countmatch>maxmatch) {
                 maxmatch=countmatch;
                 *mtx=(*tcell2)->x-x;
                 *mty=(*tcell2)->y-y;
              }
	    }
	    tcell2= &((*tcell2)->next);
	  }
	}
      }
      tcell1= &((*tcell1)->next);
    }
  }

  return maxmatch;
}

subpattern(pat1,pat2,dx,dy,gen1,gen2,count)
HashTable pat1,pat2;
int dx,dy,gen1,gen2,*count;
{
  CellList **cell,**tcell;
  int mismatch=0;

  *count=0;

  for (cell=(CellList **) pat1.table;
            cell<(CellList **) pat1.table+ pat1.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
       if ((gen1==ALLGENS || (*tcell)->gen==gen1) &&
	  !getcell((*tcell)->x+dx,(*tcell)->y+dy,gen2,pat2)) 
                                            mismatch=1;
       else (*count)++;
       tcell= &((*tcell)->next);
    } 
  } 
 
  return !mismatch;
}

delpattern(pat1,pat2,dx,dy,gen1,gen2)
HashTable pat1,pat2;
int dx,dy,gen1,gen2;
{
  CellList **cell,**tcell;

  for (cell=(CellList **) pat1.table;
       cell<(CellList **) pat1.table+ pat1.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
       if (gen1==ALLGENS || (*tcell)->gen==gen1)
	  deletecell((*tcell)->x+dx,(*tcell)->y+dy,gen2,pat2);
       tcell= &((*tcell)->next);
    } 
  } 
}


int comparegen(hash,gen1,gen2,xshift,yshift)
HashTable hash;
int gen1,gen2;
int *xshift,*yshift;
{
  CellList **cell,**tcell;
  int x1,y1,x2,y2;
  int mismatch=0;
 
  boundingrect(hash,gen1,&x1,&y1,&x2,&y2);
  boundingrect(hash,gen2,xshift,yshift,&x2,&y2);
  (*xshift)-=x1;   
  (*yshift)-=y1;

  for (cell=(CellList **) hash.table;
       !mismatch && cell<(CellList **) hash.table+ hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen1 && 
	  !getcell((*tcell)->x+(*xshift),(*tcell)->y+(*yshift),gen2,hash)) 
	{mismatch=1; break;}
      else if ((*tcell)->gen==gen2 && 
	  !getcell((*tcell)->x-(*xshift),(*tcell)->y-(*yshift),gen1,hash))
	{mismatch=1; break;}
      tcell= &((*tcell)->next);
    }
  }
  return (!mismatch);
}

int findosc(hash,gen,tmp,xshift,yshift) 
HashTable hash,tmp;
int gen;
int *xshift,*yshift;
{
int i,j;

  delgen(tmp,ALLGENS);

  copypattern(tmp,hash,0,0,1);

  for (i=1; i<=gen; i++) {
     gensparse(hash,i);
     if (comparegen(hash,1,i+1,xshift,yshift)) break;
  }

  if (i>gen) return 0;
  else return i;
}


marknghbrhoods(hash,gen)
int gen;
HashTable hash;
{
  CellList **cell,**tcell;

  /* mark neighborhoods */
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->gen==gen) {
	setcell((*tcell)->x-1,(*tcell)->y-1,SPECGEN,1,hash);
	setcell((*tcell)->x  ,(*tcell)->y-1,SPECGEN,1,hash);
	setcell((*tcell)->x+1,(*tcell)->y-1,SPECGEN,1,hash);
	setcell((*tcell)->x-1,(*tcell)->y  ,SPECGEN,1,hash);
	setcell((*tcell)->x  ,(*tcell)->y  ,SPECGEN,1,hash);
	setcell((*tcell)->x+1,(*tcell)->y  ,SPECGEN,1,hash);
	setcell((*tcell)->x-1,(*tcell)->y+1,SPECGEN,1,hash);
	setcell((*tcell)->x  ,(*tcell)->y+1,SPECGEN,1,hash);
	setcell((*tcell)->x+1,(*tcell)->y+1,SPECGEN,1,hash);
      }
      tcell= &((*tcell)->next);
    }
  }
}

interact(pat1,pat2,align,gen)
HashTable pat1,pat2,align;
int gen;
{
  CellList **cell1,**tcell1,**cell2,**tcell2;
  int dx,dy;

  marknghbrhoods(pat1,gen);
  marknghbrhoods(pat2,gen);

  for (cell1=(CellList **)pat1.table; 
       cell1<(CellList **)pat1.table+pat1.hsize; cell1++) {
    tcell1=cell1;
    while (*tcell1) {
      if ((*tcell1)->gen== SPECGEN) {
	for (cell2=(CellList **)pat2.table; 
	     cell2<(CellList **)pat2.table+pat2.hsize; cell2++) {
	  tcell2=cell2;
	  while (*tcell2) {
	    if ((*tcell2)->gen== SPECGEN) {
              dx=(*tcell2)->x-(*tcell1)->x;
              dy=(*tcell2)->y-(*tcell1)->y;
              if (!getcell(dx,dy,SPECGEN,align)) 
                        setcell(dx,dy,SPECGEN,gen,align);
	    }
	    tcell2= &((*tcell2)->next);
	  }
	}
      }
      tcell1= &((*tcell1)->next);
    }
  }

  delgen(pat1,SPECGEN);
  delgen(pat2,SPECGEN);
}


char *patstring(hash,gen,s) 
HashTable hash;
int gen;
char *s;
{

  int x1,y1,x2,y2,xmax;
  int x,y,val,i;

  boundingrect(hash,gen,&x1,&y1,&x2,&y2);

  i=0;
  if (x2<x1) s[i++]='!';
  else {
    for (y=y1; i<MAXPATLEN-2 && y<=y2; y++) { 
      for (xmax=x2; xmax>=x1; xmax--) if (getcell(xmax,y,gen,hash)) break;
      for (x=x1; i<MAXPATLEN-2 && x<=xmax; x++) {
	val=getcell(x,y,gen,hash);
        s[i++]=val?'*':'.';
      }
      s[i++]='!';
    }
  }
  s[i]='\0';

  return s;

}

FILE *loadpat(hash,gen,filename,file)
HashTable hash;
int gen;
char *filename;
FILE *file;
{
static char inpline[MAXLINE+2];
int i,j,x,y;
int nothing_read=1;

   if (!filename && !file) {
      printf("No files given\n",filename);
      exit(0);
   }

   if (!file) file=fopen(filename,"r");
   if (!file) {
      printf("Can't open %s\n",filename);
      exit(0);
   } 

   delgen(hash,ALLGENS);
   
   i=0;
   while (fgets(inpline,MAXLINE,file)) {

     /* flag that at least one line was read from file */
     nothing_read=0;     

     /* if file contains one pattern per line, just read next line */ 
     if (strchr(inpline,'!')) {  
        char *c;
      
         i=j=0; 
         for (c=inpline; *c!=' ' && *c!='\0'; c++) {
            if (*c=='!') { i++; j=0; }
            else {
              if (*c=='*') setcell(j,i,gen,1,hash);
              j++;
            }
         }

         break;
     }

     /* otherwise read entire file */
     if (sscanf(inpline,"%d %d",&x,&y)==2) {
        setcell(x,y,gen,1,hash);
      }
     else {
       for (j=0; inpline[j]!='\0'; j++) {
         if (inpline[j]=='*') setcell(j,i,gen,1,hash);
       }
       i++;
     }
   }

   if (nothing_read) {
      fclose(file);
      file=NULL;
   } 
  
   return file;

}


