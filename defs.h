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




/* various constants */
/* MAXCOL, MAXROW, DEFMR, DEFMC obsolete for list code */ 
#define MAXCOL 100  
#define MAXROW 100  
#define DEFMR 20    
#define DEFMC 20   
#define MAXLINE 512
#define MAXPATLEN 8192
#define DEFAULTHSIZE 1024
#define NOFORCE -1
#define SPECGEN -1
#define ALLGENS -2

/* use fast box-generation code */
#define USEBOXES 1

typedef struct {
   int cell[MAXCOL][MAXROW];
   int maxc,maxr;
 } TORUS;

typedef unsigned int cellseg;

#define WORDLEN 32
#define URBIT 0
#define DRBIT 1
#define ULBIT 2
#define DLBIT 3


#define dodelete(cell) {CellList *tofree; \
                       tofree= *cell; *cell=tofree->next; free(tofree);}


typedef struct {

  cellseg seg[WORDLEN];
  cellseg l,r,u,d;
  char corners; /* corner bits in positions URBIT,DRBIT,ULBIT,DLBIT */

} LifeBox;


typedef struct celllist {
   int x,y,gen;
   union {
     int val;
     LifeBox *box;
   } u;
   struct celllist *next;
 } CellList;


typedef struct {
   void **table;
   int hsize;
 } HashTable;

#define mod(a,b) ((((a)%(b))+(b))%(b))
#define div(a,b) (((a)-mod(a,b))/(b))
