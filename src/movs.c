
#include "psb0.h"

/* local function declarations ***********************************************/
static val_t chr2val   (char ch); //char to val_t used by prefcards()
static int   prefcards (psb_t *tb, char *sp); //preflop cards selection (and)

/* global function declarations **********************************************/
int psb_prcs (psb_t *tb, char *sl); //multiple selections of preflop cards (or)
int psb_mrse (psb_t *tb, char *op, float mu); //raise
int psb_mcll (psb_t *tb);		      //call
int psb_mfld (psb_t *tb);		      //fold

/*****************************************************************************/
static val_t chr2val (char ch)
{   val_t ret=0;
    switch (ch)
    {	case '\0':
	case '*': ret = 0;     break;
	case 'T': ret = PSBVT; break;
	case 'J': ret = PSBVJ; break;
	case 'Q': ret = PSBVQ; break;
	case 'K': ret = PSBVK; break;
	case 'A': ret = PSBVA; break;
	default:  ret = (ch-'0'); 
    }
    if (ret>=0 && ret<=PSBVA)
	return ret;
    else 
	return 0;
}

/*@ prefcards @****************************************************************@
 * preflop cards selection 
 *    high   hlow   comp suit conn 
 *   "[*2-A] [*2-A] [-/+,s/o,0123/c/n]"
 *   *:whatever -/+:less/great s/o:suit/off 0123/c/n/p:connec hole/all/not/par
 * examples:
 *   "Q"     all Q high 
 *   "Q*+"   Q high or greater, Q K A
 *   "*T"    all T low
 *   "QQ"    only QQ
 *   "QQ+"   QQ or greater, QQ KQ AQ KK AK AA
 *   "QQ+s"  QQ or greater but suited, KQs AQs AKs
 *   "QQ+o"  QQ or greater but offsuited
 *   "TT-on" TT or lesser but offsuited and no connected
 *   "JJ+p"  all pairs greater than JJ
 *   "22+p0" all pairs or conectors (hole=0) but greater than 22
*/
static int prefcards (psb_t *tb, char *sp)
{   if (!tb) {error(0,0,"prefcards: null pointer"); return 0;}
    if (tb->stage != PSBSTG_PR) return 0;
    int ret = 0;
    val_t h0 = tb->holds[0].v;
    val_t h1 = tb->holds[1].v;
    col_t c0 = tb->holds[0].c;
    col_t c1 = tb->holds[1].c;
    char s0[16]={0};
    short comp=0, suit=0, conn=0, chole[4]={0},all_conn=0,not_conn=0,its_pair=0;
    short rcc=0, rca=0, rcn=0, rcp=0;

    if (!sp) return 1; else strncpy(s0, sp, 16);
    remchars (s0,' ');
    val_t high = chr2val(s0[0]);
    val_t hlow = chr2val(s0[1]);
    for (int j=2; j<16 && s0[0]; j++)
    {   switch (s0[j])
	{  case '+': comp = 1; break;
	   case '-': comp =-1; break;
	   case 's': suit = 1; break;
 	   case 'o': suit =-1; break;
	   case '0': conn=chole[0] = 1; break;
	   case '1': conn=chole[1] = 1; break;
	   case '2': conn=chole[2] = 1; break;
	   case '3': conn=chole[3] = 1; break;
           case 'c': all_conn = 1; break;
           case 'n': not_conn = 1; break;
	   case 'p': its_pair = 1; break;
	}
    }
    short rh=1, rl=1, rs=1, rc=1; 
    if (high)
    {	if (comp>0) rh = (h1>=high); else if (comp<0) rh = (h1<=high); 
	else rh = (high==h1);
    }
    if (hlow)
    {	if (comp>0) rl = (h0>=hlow); else if (comp<0) rl = (h0<=hlow); 
	else rl = (hlow==h0);
    }
    if (suit>0) rs = (c0==c1); else if (suit<0) rs = (c0!=c1);
    if (conn || all_conn || not_conn || its_pair)
    {   int hole = -1;
	for (int j=0; j<4; j++)
	    if (j==(h1-h0-1) || (h1==PSBVA && j==(h0-2))) 
	    {   hole = j;
		break;
	    }
	rcc = (hole>=0 && chole[hole]); 
	rca = (hole>=0 && all_conn); 
	rcn = (hole<0 && not_conn);
	rcp = (h0==h1 && its_pair); 
	rc = (rcc || rca || rcn || rcp); 
      if (Debug(tb))
        fprintf(stderr,"prefcards:h0:%d h1:%d hole:%d rcc:%d rca:%d rcn:%d "
        "rcp:%d\n", h0, h1, hole, rcc, rca, rcn, rcp);
    }
    ret = (rh && rl && rs && rc);
    if (Debug(tb))
    {   fprintf(stderr,"prefcards: comp:%d, suit:%d, ", comp, suit);
        fprintf(stderr,"conn:%d%d%d%d,%d%d, rh:%d, rl:%d, rs:%d, rc:%d,\n", 
        chole[0],chole[1],chole[2],chole[3], not_conn,all_conn, rh,rl,rs,rc); 
    }
    Info(tb,"# prcs: sel=%s, ret=%d(%d%ds%dc%d[%da%dn%dp%d])\n", s0, ret, rh, 
        rl, rs, rc, rcc, rca, rcn, rcp);
    return ret;
}

int psb_prcs (psb_t *tb, char *sel)
{   char *tk[10];
    int j, ret=0, nt=0;
    char *sp = strdup(sel);
    nt = splittok(sp, tk, "|,"); 
    for (j=0; j<nt; j++) ret |= prefcards(tb,tk[j]);
    if (nt > 1) Info(tb,"# prcs: sel=%s, ret=%d,\n", sel, ret);
    free(sp);
    return ret;
}

//
// opt:"b" raise n big blinds (default opt=0)
// opt:"p" raise a percent of total pot (pot_t)
// opt:"s" raise a percent of stage pot (pot_s)
// opt:"a" raise all in
// 
int psb_mrse (psb_t *tb, char *opt, float mul) 
{   if (!tb) {error(0,0,"psb_mrse: null pointer"); return 0;}
    int ret = 0;
    float x = 0;
    int opb = !(!strchr(opt,'b'));
    int op0 = (!opt);
    
    if (op0) x = 0; 						      else 
    if (opb) x = (mul * tb->bl_bb); 				      else
    if (strchr(opt,'p')) x = ((mul>PSBZERO)?(mul/100):0) * tb->pot_t; else
    if (strchr(opt,'s')) x = ((mul>PSBZERO)?(mul/100):0) * tb->pot_s; else
    if (strchr(opt,'a')) {x = tb->mystk; ret = psb_push(tb,PSB_BETMAX);}

    //printf("@ opt:%s, x:%.2f, opb:%d, op0:%d\n",opt,x,opb,op0);
    if (tb->cf.sinbb && !opb) x *= tb->bl_bb;
    if (!ret) 
	ret = ( psb_wbox(tb,PSB_WRITEBBET,0,x) && psb_push(tb,PSB_BUTRAISE) ); 
    Info(tb,"# %sraising to:%.2f, o:%s, m:%.2f,\n",(ret)?"":"not ",x,opt,mul);
    return ret;
}

int psb_mcll (psb_t *tb) { return psb_push(tb,PSB_BUTCALL); }
int psb_mfld (psb_t *tb) { return psb_push(tb,PSB_FOLDCHK); }
 
/*****************************************************************************/

