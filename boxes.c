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


CellList **findcell();

LifeBox *getbox(x,y,hash)
int x,y;
HashTable hash;
{
  CellList **cell;
  LifeBox *box;

  cell=findcell(x,y,SPECGEN,hash);

  if (!(*cell)) {
    int i;    

    (*cell)=(CellList *)malloc(sizeof(CellList));
    (*cell)->x=x;
    (*cell)->y=y;
    (*cell)->gen=SPECGEN;
    (*cell)->next=NULL;

    box=(*cell)->u.box=(LifeBox *)malloc(sizeof(LifeBox));
    for (i=0; i<WORDLEN; i++) box->seg[i]=0;
    box->l=0;
    box->r=0;
    box->u=0;
    box->d=0;
    box->corners='\0';
  } 
  else box=(*cell)->u.box;

  return box;
}

void delbox(x,y,hash)
int x,y;
HashTable hash;
{
  CellList **cell;

  cell=findcell(x,y,SPECGEN,hash);
  free((*cell)->u.box);
  deletecell(x,y,SPECGEN,hash);
}


HashTable makeboxes(listhash,gen,transx,transy)
HashTable listhash;
int gen;
int *transx,*transy;
{
  CellList **cell,**tcell;
  HashTable boxhash;
  int boxx,boxy,dx,dy,x,y;
  LifeBox *box;
  int x1,y1,x2,y2;

  /* center the pattern at box center so small patterns start in one box */
  boundingrect(listhash,gen,&x1,&y1,&x2,&y2);
  *transx=WORDLEN/2-(x1+x2)/2;
  *transy=WORDLEN/2-(y1+y2)/2;

  allochash(&boxhash,DEFAULTHSIZE);

  for (cell=(CellList **)listhash.table; 
       cell<(CellList **)listhash.table+listhash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      if ((*tcell)->u.val && (gen==ALLGENS || (*tcell)->gen==gen) ) {

        x=(*tcell)->x+ *transx;
        y=(*tcell)->y+ *transy;

	boxx=div(x,WORDLEN);
	dx=WORDLEN-1-mod(x,WORDLEN);
	boxy=div(y,WORDLEN);
	dy=mod(y,WORDLEN);

        box=getbox(boxx,boxy,boxhash);
        box->seg[dy]|=(1<<dx);

      }
      tcell= &((*tcell)->next);
    }
  }


  return boxhash;
}

restorecells(boxhash,listhash,gen,transx,transy)
HashTable boxhash,listhash;
int gen,transx,transy;
{
  CellList **cell,**tcell;
  int dx,dy;

  for (cell=(CellList **)boxhash.table; 
       cell<(CellList **)boxhash.table+boxhash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      for (dx=0; dx<WORDLEN; dx++)
	for (dy=0; dy<WORDLEN; dy++) {
	  if ((*tcell)->u.box->seg[dy]&(1<<dx))
           setcell((*tcell)->x*WORDLEN+WORDLEN-1-dx-transx,
                   (*tcell)->y*WORDLEN+dy-transy,
		    gen,1,listhash);
	}
      tcell= &((*tcell)->next);
    }

  }

}

clearboxes(boxhash)
HashTable boxhash;
{
  CellList **cell,**tcell;

  for (cell=(CellList **)boxhash.table; 
       cell<(CellList **)boxhash.table+boxhash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      free((*tcell)->u.box);
      dodelete(tcell);
    }
  }
}

void genlist(listhash,gen,k) 
int gen,k;
HashTable listhash;
{
HashTable boxes;
int tx,ty;
int i;
  
    /* transfer list to box representation */
    boxes=makeboxes(listhash,gen,&tx,&ty);
    delgen(listhash,gen);

    /* generate k steps */
    for (i=0; i<k; i++) genboxes(boxes);

    /* restore list representation */
    restorecells(boxes,listhash,gen+k,tx,ty);
    clearboxes(boxes);
    free(boxes.table);
    
}

cellseg dobox();

#ifdef TRACEBOXES
  int boxcount;
#endif

genboxes(hash)
HashTable hash;
{
  CellList **cell,**tcell;


#ifdef TRACEBOXES
   boxcount=0;
#endif

  /* assumes neighborhood information is initially 0 */

  /* pass neighborhood info to appropriate cells */
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) { 
      tellneighbors(tcell,hash);
      tcell= &((*tcell)->next);
#ifdef TRACEBOXES
      boxcount++;
#endif
     }
  }

  /* compute next generation */
  for (cell=(CellList **)hash.table; 
       cell<(CellList **)hash.table+hash.hsize; cell++) {
    tcell=cell;
    while (*tcell) {
      /* generate box; delete if empty */
      if (!dobox((*tcell)->u.box)) {
	free((*tcell)->u.box);
	dodelete(tcell)
	}
       else tcell= &((*tcell)->next);
    }
  }

#ifdef TRACEBOXES
      fprintf(stderr,"boxcount=%d\n",boxcount);
#endif
}

tellneighbors(tcell,hash)
CellList **tcell;
HashTable hash;
{
int x,y,i;
LifeBox *mybox,*nbrbox;
cellseg(tseg);

       x=(*tcell)->x;
       y=(*tcell)->y;
       mybox=(*tcell)->u.box;

       if (mybox->seg[0]) {
	 nbrbox=getbox(x,y-1,hash);
	 nbrbox->d=mybox->seg[0];
       }

       if (mybox->seg[WORDLEN-1]) {
	 nbrbox=getbox(x,y+1,hash);
	 nbrbox->u=mybox->seg[WORDLEN-1];
       }

       tseg=0;
       for (i=0; i<WORDLEN; i++) 
               tseg|=((mybox->seg[i]&(1<<WORDLEN-1))>>WORDLEN-1)<<i;
       if (tseg) {
	 nbrbox=getbox(x-1,y,hash);
	 nbrbox->r=tseg;
       }

       tseg=0;
       for (i=0; i<WORDLEN; i++) 
               tseg|=(mybox->seg[i]&1)<<i;
       if (tseg) {
	 nbrbox=getbox(x+1,y,hash);
	 nbrbox->l=tseg;
       }

       if (mybox->seg[0]&1) { 
          nbrbox=getbox(x+1,y-1,hash);
          nbrbox->corners|=(1<<DLBIT);
       }

       if (mybox->seg[0]&(1<<WORDLEN-1)) { 
	 nbrbox=getbox(x-1,y-1,hash);
	 nbrbox->corners|=(1<<DRBIT);
       }

       if (mybox->seg[WORDLEN-1]&1) { 
	 nbrbox=getbox(x+1,y+1,hash);
	 nbrbox->corners|=(1<<ULBIT);
       }

       if (mybox->seg[WORDLEN-1]&(1<<WORDLEN-1)) { 
          nbrbox=getbox(x-1,y+1,hash);
          nbrbox->corners|=(1<<URBIT);
       }
}

/* boolean life function for bitwise operations */
#define ha0(a,b) {b^=a; a|=b;}
#define ha1(a,b) {b^=a; a&=b;}
#define dolife(b8,b0,b1,b2,b3,b4,b5,b6,b7) \
      ha0(b7,b6);\
      ha0(b5,b4);\
      ha0(b3,b2);\
      ha0(b1,b0);\
      ha0(b2,b0);\
      ha1(b0,b4);\
      ha0(b6,b4);\
      ha1(b7,b6);\
      ha1(b3,b2);\
      ha1(b5,b6);\
      ha1(b1,b2);\
      ha1(b0,b2);\
      ha0(b6,b2);\
      b0|=b1;\
      b0|=b3;\
      b5|=b7;\
      ha0(b5,b0);\
      b0&=b6;\
      b0^=b5;\
      b8|=b4; \
      b8&= ~b0;\
      b8&=b2;



#define bit(x,i) (((x)>>(i))&1)

/* perform generation on box with correct neighborhood info.
   zero neighborhood info for next pass.
   return bitwise or of box rows */
cellseg dobox(box)
LifeBox *box;
{
  cellseg cell,nu,nd,nl,nr,nur,nul,ndr,ndl;
  cellseg tnu,tnur,tnul,tnr,tnl,tcell;
  cellseg orcell=0;
  int i;


  /* compute transition */

  cell=box->seg[0];
  nu=box->u;

  nr=(cell<<1)+bit(box->r,0);
  nl=(cell>>1)+(bit(box->l,0)<<WORDLEN-1);
  nur=(nu<<1)+bit(box->corners,URBIT);
  nul=(nu>>1)+(bit(box->corners,ULBIT)<<WORDLEN-1);

  /* worth unrolling it? */
  for (i=0; i<WORDLEN-1; i++) {
    nd=box->seg[i+1];
    ndr=(nd<<1)+bit(box->r,i+1);
    ndl=(nd>>1)+(bit(box->l,i+1)<<WORDLEN-1);

    tnul=nl;    tnu=cell;     tnur=nr;
    tnl=ndl;    tcell=nd;     tnr=ndr;

    dolife(cell,nu,nd,nl,nr,nur,nul,ndr,ndl);
    box->seg[i]=cell;
    orcell|=cell;       

    nul=tnul; nu=tnu; nur=tnur; nl=tnl; nr=tnr; cell=tcell;
       
  }

  nd=box->d;
  ndr=(nd<<1)+bit(box->corners,DRBIT);
  ndl=(nd>>1)+(bit(box->corners,DLBIT)<<WORDLEN-1);
  dolife(cell,nu,nd,nl,nr,nur,nul,ndr,ndl);
  box->seg[i]=cell;
  orcell|=cell;


  /* clear neighborhood info for next pass */
  box->u=box->d=box->l=box->r=0;
  box->corners='\0';


  return orcell;
}





