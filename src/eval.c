
#include "psb0.h"

/* local function declarations ***********************************************/
static rnk_t eval_fl (card_t *cs, card_t *hn); //flush
static rnk_t eval_st (card_t *cs, card_t *hn); //straight, 
static rnk_t eval_sf (card_t *cs, card_t *hn, rnk_t rs, rnk_t rf); //fl-st,royal
static rnk_t eval_4k (card_t *cs, card_t *hn); //4 of kind
static rnk_t eval_fh (card_t *cs, card_t *hn); //full house
static rnk_t eval_nk (card_t *cs, card_t *hn); //everything else
					       //return number of needed cards
static int draw_st (card_t *cs, card_t *hn, stg_t stg); //flush draw 
static int draw_st (card_t *cs, card_t *hn, stg_t stg); //straight draw

static int eval_cards (card_t *hl, card_t *hn, rnk_t rk); 

/* global function declarations **********************************************/
int eval_eval0 (psb_t *tb);

/*****************************************\**********************************\/
| cs:cards(ascending order), hn:best hand |
\----------------------------------------*/
static rnk_t eval_fl (card_t *cs, card_t *hn) 
{   int i, j, c;
    int colors[5]={0}; //color counters
    
    if (!cs[4].v) return 0; 
    for (j=0; j<7; j++) colors[ cs[j].c ]++;
    for (c=1; c<5; c++) 
    {	if (colors[c] >= 5) 
        {   for (i=6,j=0; i>=0&&j<5; i--) if (cs[i].c == c) hn[j++] = cs[i]; 
	    return PSBRNK_FL;
        }
    }
    return 0;
}

static rnk_t eval_st (card_t *cs, card_t *hn) 
{   int j, v, s;
    int values[15]={0};
    
    if (!cs[4].v) return 0;
    for (j=0; j<7; j++) values[ cs[j].v ] = 1+j;
    if (values[14]) values[1] = values[14];	
    for (v=14,s=0; v>0; v--)
    {   if (values[v]) s++; else s=0;
	if (s >= 5)
	{   for (j=4; j>=0; j--) hn[j] = cs[ values[v++]-1 ];
	    return PSBRNK_ST;
	}
    }
    return 0;
}

static rnk_t eval_sf (card_t *cs, card_t *hn, rnk_t rs, rnk_t rf) 
{   int j, k, s, suit=0;
    int colors[5]={0};
    int values[15]={0};
        
    if (!(rs && rf)) return 0;
    for (j=0; j<7; j++) colors[ cs[j].c ]++;
    for (k=1; k<5; k++) if (colors[k] >= 5) suit = k; if (!suit) return 0;
    for (j=0; j<7; j++) if (cs[j].c == suit) values[ cs[j].v ] = 1+j;
    if (values[14]) values[1] = values[14];	
    for (k=14,s=0; k>0; k--)
    {   if (values[k]) s++; else s=0;
	if (s == 5)
	{   for (j=4; j>=0; j--) hn[j] = cs[ values[k++]-1 ];
            return (hn[0].v == PSBVA)? PSBRNK_RF: PSBRNK_SF; 
	}
    }
    return 0;
}

static rnk_t eval_4k (card_t *cs, card_t *hn) 
{   int i, j, v;
    int values[15]={0};
    
    if (!cs[3].v) return 0;
    for (i=0;  i<7; i++) values[ cs[i].v ]++;
    for (v=14; v>0; v--)
    {	if (values[v] == 4) 
    	{   for (i=0,j=0; i<7; i++) 
    	        if (cs[i].v == v) hn[j++] = cs[i]; //copy card
	        else if (cs[i].v) hn[ 4 ] = cs[i]; //kicker
	    return PSBRNK_4K;
	}	
    }
    return 0;
}

static rnk_t eval_fh (card_t *cs, card_t *hn) 
{   int i, j, k, v, vt=0, vp=0;
    int values[15]={0};
    int p[3]={0}, pc=0; //pairs, counter
    int t[3]={0}, tc=0; //3kind, counter

    if (!cs[4].v) return 0;
    for (i=0; i<7; i++) values[ cs[i].v ]++;
    for (v=14; v>0; v--) 
        if (values[v] == 3) t[ tc++ ] = v; else 
	if (values[v] == 2) p[ pc++ ] = v;
    if ((tc>0 && pc>0) || tc>1)
    {   if (tc == 2) { vt=t[0]; vp=t[1]; } else 
	if (tc == 1) { vt=t[0]; vp=p[0]; }
	for (i=6,j=0,k=3; i>=0&&k+j<8; i--) 
	    if (cs[i].v == vt) hn[j++] = cs[i]; else //3kind
	    if (cs[i].v == vp) hn[k++] = cs[i];      //pair
	return PSBRNK_FH;
    }
    return 0;
}

static rnk_t eval_nk (card_t *cs, card_t *hn) 
{   int i, j, k, v;
    int values[15]={0};
    int p[3]={0}, pc=0; //pairs, counter
    int t[3]={0}, tc=0; //3kind, counter
    
    if (!cs[0].v) return 0;
    for (i=0; i<7; i++) values[ cs[i].v ]++;
    for (v=14; v>0; v--) 
	if (values[v]==3) t[ tc++ ] = v; else 
	if (values[v]==2) p[ pc++ ] = v;
    if (tc>=1 && pc) return 0;
    else if (tc==1 && cs[2].v)
    {	for (i=6,j=0,k=3; i>=0; i--) 
    	    if (cs[i].v==t[0] && j<3) hn[j++] = cs[i]; else //3kind
            if (cs[i].v       && k<5) hn[k++] = cs[i];      //kickers
	return PSBRNK_3K;
    }
    else if (pc>=2 && cs[3].v)
    {   for (i=0,j=0,k=2; i<7; i++)
	    if (cs[i].v == p[0]) hn[j++] = cs[i]; else 	    //1th pair
	    if (cs[i].v == p[1]) hn[k++] = cs[i]; else      //2th pair	
	    if (cs[i].v) 	 hn[ 4 ] = cs[i];           //kicker
	return PSBRNK_2P;
    }
    else if (pc==1 && cs[1].v)
    {   for (i=6,j=0,k=2; i>=0; i--) 
	    if (cs[i].v==p[0] && j<2) hn[j++] = cs[i]; else //pair
	    if (cs[i].v       && k<5) hn[k++] = cs[i];      //kickers
	return PSBRNK_1P;
    }
    else 
    {   for (i=6,k=0; i>=0; i--) if (cs[i].v && k<5) hn[k++] = cs[i]; 
	return PSBRNK_HC; 
    }
    return 0; /* if all right, we'll never come to here :p */
}

static int eval_cards (card_t *holds, card_t *bhand, rnk_t hrank) 
{   val_t v0=holds[1].v; //high 
    val_t v1=holds[0].v;
    val_t h0,h1,h2,h3,h4;
    col_t c,c0,c1;
    
    if (!holds || !bhand || !hrank) return 0;
    switch (hrank) 
    { case PSBRNK_RF: /*10*/		return 9; 
      case PSBRNK_SF: /*9*/
	  h0= bhand[0].v;
	  h4= bhand[4].v;
	  if ((v0>h0 || v0<h4) && (v1>h0 || v1<h4)) //none in sf, sf in comms
	      				return 5; 
	  else if (v0<=h0 && v0>=h4 && v1<=h0 && v1>=h4) //two in sf
	      				return 9;  
	  else 				return 8; 
          break; 

      case PSBRNK_4K: /*8*/
	  h0= bhand[0].v; 
	  h4= bhand[4].v; //kicker
	  if (v0==h0 || v1==h0 || h4 == PSBVA)   //one in 4k or A kicker
	  				return 8; 
	  else if (v0 >= PSBVQ)		return 7; //
	  else if (v0 >= PSBVT)		return 5; //kicker, 4k in comms
	  else if (v0 >= PSBV8)		return 3; //
          break;

      case PSBRNK_FH: /*7*/
	  h0= bhand[0].v; 
	  h3= bhand[3].v;
	  if (v0!=h0 && v0!=h3 && v1!=h0 && v1!=h3) //none in fh, fh in comms
	  {   if      (h3 >= PSBVK)	return 6; 
	      else if (h3 >= PSBVT)	return 4; 
	      else if (h3 >= PSBV6)	return 2; 
	  } 					  //one in trey, one in pair
	  else if ((v0==h0 || v0==h3) && (v1==h0 || v1==h3)) 
	  {   if      (h0 > h3)		return 7;  
	      else			return 6; 
	  } 				
	  else if (v0==h0 || v1==h0)		  //at least one in trey
	  {   if      (h0 > h3) 	return 6; 
	      else			return 4; 
	  } 
	  else if (v0==h3 || v1==h3)		  //at least one in pair
	  {   if      (h3 > PSBVT)	return 6; 
	      else if (h3 > PSBV6)	return 4; 
	      else			return 2; 
	  }
	  break;
	
      case PSBRNK_FL: /*6*/
	  c= bhand[0].c;
	  c0= holds[1].c;
	  c1= holds[0].c;
	  if      (c0!=c && c1!=c) 	return 2; //none in fl, fl in comms
	  else if (c0==c && c1==c)		  //two in fl
	  {   if      (v0 >= PSBVQ) 	return 6;   
	      else if (v0 >= PSBVT) 	return 5; 
	      else if (v0 >= PSBV7) 	return 4; 
	      else 			return 2; 
	  }
	  else if (c0==c || c1==c)		  //one in fl
	  {   if      (v0 >= PSBVK)	return 6; 
	      else if (v0 >= PSBVQ) 	return 5; 
	      else if (v0 >= PSBVT)	return 4; 
	      else if (v0 >= PSBV7)	return 2;    
	  }
	  break;
	  
      case PSBRNK_ST: /*5*/
	  h0= bhand[0].v;
	  h1= bhand[1].v;
	  h2= bhand[2].v;
	  h3= bhand[3].v;
	  h4= bhand[4].v;
	  if ((v0>h0 || v0<h4) && (v1>h0 || v1<h4)) //none in st, st in comms
	  {   if      (h0 == PSBVA)	return 5; 
	      else			return 3; 
	  } 					    
#define eq(p,q) (v0==(p) && v1==(q))
#define ne(p,q) (v0!=(p) && v1!=(q))
#define or(p,q) (v0==(p) || v1==(q))		//two holds in st		 
	  else if (v0<=h0 && v0>=h4 && v1<=h0 && v1>=h4) 
	  {   if     ((h0 == PSBVA) //all st to as 
		    || eq(h0,h1)) 	return 5; 
	      else if (eq(h2,h3) || eq(h1,h2) || eq(h0,h2) || eq(h2,h4) 
		    || eq(h1,h3)) 	return 5; 
	      else if (eq(h3,h4) || eq(h0,h3) || eq(h1,h4) )
	  				return 4; 
	      else if (eq(h0,h4)) 	return 3; 
	  } 					//one hold in st
	  else if ((v0<=h0 || v0>=h4) || (v1<=h0 || v1>=h4)) 
	  {   if      (h0 == PSBVA)	return 5; 
	      else if (h0 >= PSBVQ)	return 5; 
	      else if (or(h0,h0)) 	return 4; 
	      else 			return 2; 
	  }
	  break;

      case PSBRNK_3K: /*4*/
	  h0= bhand[0].v;
	  h3= bhand[3].v;
	  h4= bhand[4].v;
	  if 	  (ne(h0,h0))  	//none in 3k, 3k in comms
	  {   if      (v1>=PSBVQ)	return 4;
	      else if (v1>=PSBVJ)	return 3;
	      else if (v1>=PSBV9)	return 2; 
	  } 
	  else if (eq(h0,h0)) 	//two in 3k, 
	  {   if (h0>h3 && h0>h4)  	return 4; 
	      else			return 3; 
	  } 
	  else if (or(h0,h0))	//one in 3k, 
	  {   if (h0>h3 && h0>h4) 	return 4; 
	      else if (v0>=PSBVQ)	return 3;
	      else 			return 2;
	  }
	  break;
      
      case PSBRNK_2P: /*3*/
	  h0= bhand[0].v;
	  h2= bhand[2].v;
	  h4= bhand[4].v;
	  if  	  (ne(h0,h2))	     //none in 2p, they're in comms
	  {   if      (v0 >= PSBVT)	return 1; 
	  }
	  else if (eq(h0,h2)) 	     //two in 2p
	  {   if      (h0>h4 && h2>h4) 	return 3;  
	      else 	 		return 2; 
	  }
	  else if ((v0==v1 && v0==h0) || or(h0,h0)) //one high pair
	  {   if      (h0>h4 && h2>h4) 	return 2; 
	      else if (h0 > h4)		return 1; 
	  }
	  else if ((v0==v1 && v0==h2) || or(h2,h2)) //one low pair
	  {   if      (h2 > h4)		return 1; 
	  }
	  break;

      case PSBRNK_1P: /*2*/
	  h0= bhand[0].v;
	  h2= bhand[2].v;
	  h3= bhand[3].v;
	  if (or(h0,h0)) //at least one in holds
	  {   if      (h0>=h2) 		return 2; //top pair
	      else if (h0>=h3)		return 1; //medium pair
	  }
	  break;

      case PSBRNK_HC: /*1*/
          h0= bhand[0].v;
	  if (or(h0,h0))		return 1; //high card
	  break;
	 
      default : ;
    }
#undef eq
#undef ne
#undef or
    return 0; 
}

/*****************************************\**********************************\/
| cs:cards(ascending order), hn:best draw |
\----------------------------------------*/
static int draw_fl (card_t *cs, card_t *hn, stg_t stg) 
{   int j, k, c3=0, c4=0, n=0, r=0;
    int colors[5]={0}; //colors counters
    card_t xcs[15]={0}, nc={0};

    for (j=0; j<5; j++) hn[j] = nc;
    //if (!cs[4].v) return 0; //only posflop
    if (!cs[2].v || stg==PSBSTG_RI) return 0; //at least 3cards
    for (j=0; j<7; j++) colors[ cs[j].c ]++;
    for (k=1; k<5; k++) 
	switch (colors[k])
	{   case 3: if (stg<=PSBSTG_FL) c3=k; r=2; break;
	    case 4: c4=k; r=1; break;
	    case 5: return 0;
	}
    k = (c4)? c4: c3;
    //printf("c3=%d c4=%d k=%d\n",c3,c4,k);
    if (k) 
    {   for (j=0; j<7; j++) if (cs[j].c == k)  xcs[ cs[j].v ] = cs[j];
	for (j=14; j>0; j--) 
	    if ( !xcs[j].v ) 
	    {   xcs[j].v = j; 
		xcs[j].c = k; 
		n++; 
		if (c4 && n) break; else if (c3 && (n==2)) break; 
	    }
        for (j=14,k=0; j>0&&k<5; j--) if (xcs[j].v) hn[k++] = xcs[j]; 
	//for (j=14; j>0; j--) printf("%d,%d, ",xcs[j].v,xcs[j].c); printf("\n");
    }
    return (r)? r : 0;
}

static int draw_st (card_t *cs, card_t *hn, stg_t stg) 
{   int j, k, s0=0, s1=0, h1=0, h2=0;
    card_t xcs[15]={0}, nc={0};
    
    for (j=0; j<5; j++) hn[j] = nc;
    if (!cs[4].v || stg==PSBSTG_RI) return 0;
    for (j=0; j<7; j++) {xcs[ cs[j].v ] = cs[j];} xcs[1] = xcs[14];
    //for (j=14; j>0; j--) printf("%d,%d, ",xcs[j].v,xcs[j].c); printf("\n");
    for (k=14; k>4; k--)
    {	s0=s1=0;
	for (j=k; j>k-5; j--) 
 	{   if ( xcs[j].v ) s1++; else s0++; 
	    if (s1==5) return 0;  
	    if (stg<=PSBSTG_FL && s0==3) break; else
	    if (stg==PSBSTG_TU && s0==2) break; 
	    if ((s0+s1) == 5) {if (s0==1&&!h1) h1=j; else if (s0==2&&!h2) h2=j;}
	    //printf(" j=%d k=%d s0=%d s1=%d h1=%d h2=%d\n",j,k,s0,s1,h1,h2);
        }
    }
    j = (h1)? h1: h2;
    if (j)
    {   for (k=4; k>=0; k--) 
	{   if ( !xcs[j].v ) 
	    {   hn[k].v = j; 
		hn[k].c = (xcs[j-1].c <= 1)? 4 : xcs[j-1].c-1;
	    }
	    else hn[k] = xcs[j];
	    j++;
	}
    }
    //for (j=14; j>0; j--) printf("%d,%d, ",xcs[j].v,xcs[j].c); printf("\n");
    if (h1) j=1; else if (h2) j=2; else j=0;
    return (j)? j: 0;
}

/*****************************************************************************/
int eval_eval0 (psb_t *tb)
{   if (!tb) {error(0,0,"eval_eval0: null pointer"); return 0;}
    card_t hs[2]={0}, cs[7]={0}, hn[5]={0}, hst[5]={0}, hfl[5]={0};
    rnk_t rank=0, rst=0, rfl=0;
    int sof = sizeof(card_t);

    memcpy(cs, tb->sdrac, 7*sof);
    memcpy(hs, tb->holds, 2*sof); 
    
    rst = eval_st(cs, hst);
    rfl = eval_fl(cs, hfl);
    if ( (rank= eval_sf(cs, hn,rst,rfl)) ) memcpy(hn, hst, 5*sof); else
    if ( (rank= eval_4k(cs, hn))	 ) ;         		   else
    if ( (rank= eval_fh(cs, hn))	 ) ;         		   else
    if ( (rank= rfl)			 ) memcpy(hn, hfl, 5*sof); else
    if ( (rank= rst)		   	 ) memcpy(hn, hst, 5*sof); else
    if ( (rank= eval_nk(cs, hn))	 ) ;		
    EVP(tb)->hrank = rank;
    EVP(tb)->heval = eval_cards(hs, hn, rank);
    memcpy(EVP(tb)->bhand, hn, 5*sof);
    
    int fl, st, efl=0, est=0;
    if ((fl = draw_fl(cs,hfl,tb->stage))) efl = eval_cards(hs,hfl,PSBRNK_FL);
    if ((st = draw_st(cs,hst,tb->stage))) est = eval_cards(hs,hst,PSBRNK_ST);
    EVP(tb)->fldrw=fl; EVP(tb)->flevl=efl; memcpy(hfl,EVP(tb)->flhnd,5*sof);
    EVP(tb)->stdrw=st; EVP(tb)->stevl=est; memcpy(hst,EVP(tb)->sthnd,5*sof);
/*    
    int j;
    printf("fldrw:%d flevl:%d flhnd:", fl, efl);
    for (j=0; j<5; j++) 
	printf("%s%s ", val2str(hfl[j].v), col2str(hfl[j].c)); printf("\n");
    printf("stdrw:%d stevl:%d sthnd:", st, est);
    for (j=0; j<5; j++) 
	printf("%s%s ", val2str(hst[j].v), col2str(hst[j].c)); printf("\n");
*/
    return 1;
}

/*****************************************************************************/
//#define EVAL0 
#ifdef  EVAL0
rnk_t eval0 (card_t *cs, card_t *hn)
{   card_t hst[5]={0}, hfl[5]={0};
    rnk_t rank=0, rst=0, rfl=0;
    int sof = sizeof(card_t);

    rst = eval_st(cs, hst);
    rfl = eval_fl(cs, hfl);
    if ( (rank= eval_sf(cs, hn,rst,rfl)) ) memcpy(hn, hst, 5*sof); else
    if ( (rank= eval_4k(cs, hn))	 ) ;         		   else
    if ( (rank= eval_fh(cs, hn))	 ) ;         		   else
    if ( (rank= rfl)			 ) memcpy(hn, hfl, 5*sof); else
    if ( (rank= rst)		   	 ) memcpy(hn, hst, 5*sof); else
    if ( (rank= eval_nk(cs, hn))	 ) ;		
    return rank;
}

int sort0 (card_t *cs, int len, int cmp) 
{   int i, j;  
    card_t ax;       
    if (!cs || len<0) return 0;
    for (i=0; i<len-1 && cs[i].v; i++) 
    for (j=i+1; j<len && cs[j].v; j++)     
        if ( (cmp)? (cs[i].v < cs[j].v) : (cs[i].v > cs[j].v) )
        {   ax = cs[i];
            cs[i] = cs[j];
            cs[j] = ax;
        }
    return 1;
}

#define DECK0 52
int main (int argc, char **argv)
{   card_t dk[ DECK0 ];
    int j,k, a,b,c,d,e,f,g, rc[11]={0}, n=0, m=0;
    card_t cs[7]={0}, hn[5]={0};
    rnk_t rank=0;
    char *C=" shdc";
    char *V=" 123456789TJQKA";
    char *R[]={"xx","HC","1P","2P","3K","ST","FL","FH","4K","SF","RF"};
    char s0[24]={0};

/**/
    printf("# 7 cards poker evaluator.\n");
    if (argc==1) 
    {   printf("# usage: %s [holds] comms;  eg: %s As Kc  Qd Jh Ts 9c 8d;\n", 
	    	argv[0], argv[0]);
        exit(1);
    }
    for (j=1; j<argc; j++) 
    {   
#define ifItsOk(x,n) (((x)>0 && (x)<=(n))? (x) : 0) 
	sprintf(s0,"%c",argv[j][0]); cs[j-1].v = ifItsOk(strstr(V,s0)-V,14);
        sprintf(s0,"%c",argv[j][1]); cs[j-1].c = ifItsOk(strstr(C,s0)-C, 4);
        //printf("%d %d %s: %d %d\n",j,argc,argv[j],cs[j-1].v,cs[j-1].c);
        if (!cs[j-1].v)
	{ printf("@ sorry but you entered bad value of cards!\n\n"); exit(1); }
    }
    sort0(cs,7,0);
    rank = eval0(cs,hn);
    printf("@ sort: "); 
    for (j=0; j<7; j++) printf("%c%c ", V[cs[j].v], C[cs[j].c]); printf("\n");
    printf("@ rank: %s, best hand: ", R[rank]);
    for (j=0; j<5; j++) printf("%c%c ", V[hn[j].v], C[hn[j].c]); printf("\n");

    card_t hfl[5]={0}, hst[5]={0}, *hp=0;
    int fl, st, efl=0, est=0;
    if ( (fl = draw_fl(cs, hfl, 0)) ) efl = eval_cards(cs, hfl, PSBRNK_FL);
    if ( (st = draw_st(cs, hst, 0)) ) est = eval_cards(cs, hst, PSBRNK_ST);
    printf("@ flush draw:%d  eval:%d  fake hand:", fl, efl); hp=hfl;
    for (j=0; j<5; j++) printf("%c%c ", V[hp[j].v], C[hp[j].c]); printf("\n");
    printf("@ strai draw:%d  eval:%d  fake hand:", st, est); hp=hst;
    for (j=0; j<5; j++) printf("%c%c ", V[hp[j].v], C[hp[j].c]); printf("\n\n");

    return 0;    
/**/
/*
    printf("# enumerate and tabulate 7/6/5 poker hands.\n");
    for (j=0; j<DECK0; j++) 
    {   dk[j].c = (j %  4) + 1; 
	dk[j].v = (j % 13) + 2; 
	//printf("dk[%02d] = %s%s\n", j, val2str(dk[j].v), col2str(dk[j].c));
	//return 0;
    }
    for (a=0; 	a<DECK0; a++)
    for (b=a+1; b<DECK0; b++)
    for (c=b+1; c<DECK0; c++)
    for (d=c+1; d<DECK0; d++)
    for (e=d+1; e<DECK0; e++) 
    for (f=e+1; f<DECK0; f++) 
    for (g=f+1; g<DECK0; g++) 
    {   n++;
	cs[0] = dk[ a ];
       	cs[1] = dk[ b ];
       	cs[2] = dk[ c ];
       	cs[3] = dk[ d ];
       	cs[4] = dk[ e ];
       	cs[5] = dk[ f ];  
       	cs[6] = dk[ g ];  
   	rank = eval0(cs,hn);
	rc[ rank ]++;
	//if (rank == PSBRNK_ST)
	if (!(n%1000000)) printf("%d million tested hands!!\n",++m); if (0)
	{   for (j=0; j<5; j++) 
	        //printf("%s%s ", V[cs[j].v], C[cs[j].c]);
	        printf("%s%s ", V[hn[j].v], C[hn[j].c]);
            printf("\n");
	}
    }
    for (j=1; j<11; j++) printf("%s: %d\n", R[j], rc[j]);
    printf("n = %d\n", n);
    return 0;
*/
}

/*
5 cards 
Poker Hand            Nbr. of Hands     Probability
High card only            1,302,540      0.50117739
One pair                  1,098,240      0.42256903
2 pairs                     123,552      0.04753902
3 of a kind                  54,912      0.02112845
Straight                     10,200      0.00392464
Flush                         5,108      0.00196540
Full House                    3,744      0.00144058
4 of a kind                     624      0.00024010
Other straight flush             36      0.00001385
Royal straight flush              4      0.00000154
Total =                   2,598,960      1.00000000

6 cards 
Poker Hand            Nbr. of Hands     Probability
High card only            6,612,900      0.32482224
One pair                  9,730,740      0.47796893
2 pairs                   2,532,816      0.12441062
3 of a kind                 732,160      0.03596332
Straight                    361,620      0.01776258
Flush                       205,792      0.01010840
Full House                  165,984      0.00815305
4 of a kind                  14,664      0.00072029
Other straight flush          1,656      0.00008134
Royal straight flush            188      0.00000923
Total =                  20,358,520      1.00000000

7 cards
Poker Hand            Nbr. of Hands     Probability
High card only           23,294,460      0.17411920
One pair                 58,627,800      0.43822546
2 pairs                  31,433,400      0.23495536
3 of a kind               6,461,620      0.04829870
Straights                 6,180,020      0.04619382
Flush                     4,047,644      0.03025494
Full House                3,473,184      0.02596102
4 of a kind                 224,848      0.00168067
Other straight flush         37,260      0.00027851
Royal straight flush          4,324      0.00003232
Total =                 133,784,560      1.00000000
*/

#endif /* EVAL0 */
/*****************************************************************************/

