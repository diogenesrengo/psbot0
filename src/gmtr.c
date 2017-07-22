
#include <math.h>
#include "psb0.h" 

/* local function declarations. **********************************************/
static void gmtr_init (psb_t *tb); //geometries initialize and calculate

/* global function declarations. *********************************************/
int gmtr_calc0 (psb_t *tb); //reset geometries if window size changes

/* geometries initialization. ************************************************/
static void gmtr_init (psb_t *tb)
{
//-----------------------------------------------------------------------/
#define TEMPWINW 792 //initial values are according to this window size  |
#define TEMPWINH 546 //later are translated to a general fractional form |
//-----------------------------------------------------------------------/
#define init(_id_,tempx,tempy,tempw,temph) \
{   gmtx_t gm = { #_id_ }; \
    gm.fg.x = (float)tempx/TEMPWINW; gm.fg.y = (float)tempy/TEMPWINH; \
    gm.fg.w = (float)tempw/TEMPWINW; gm.fg.h = (float)temph/TEMPWINH; \
    gm.x = roundf( gm.fg.x*Win_w(tb) ); \
    gm.y = roundf( gm.fg.y*Win_h(tb) ); \
    gm.w = roundf( gm.fg.w*Win_w(tb) ); \
    gm.h = roundf( gm.fg.h*Win_h(tb) ); \
    GMP(tb)->_id_ = gm; \
    if ((gm.x + gm.w) > TEMPWINW || (gm.y + gm.h) > TEMPWINH) \
    {   fprintf(stderr,"# gmtr: %s is out of window's limits.\n", gm.id); \
	error(3,0,"sorry, can't go on if there're bad geometries"); \
    } \
    if (Debug(tb)) \
      fprintf(stderr,"gmtr: id=%s gm=%d,%d,%d,%d, fg=%f,%f,%f,%f, \n", \
        gm.id, gm.x,gm.y,gm.w,gm.h, gm.fg.x,gm.fg.y,gm.fg.w,gm.fg.h ); \
}
    init (tocll,    524, 515, 115, 20); //amount in button call
    init (torse,    661, 515, 115, 20); //amount in button raise
    init (pot_t,    371, 150, 100, 16); //total pot, above
    init (pot_s,    331, 267, 100, 16); //stage pot, bottom
  //  init (pot_p,    432, 267,  80, 16); //split pot 
    init (vcard[0], 332, 340, 100, 30); //holds cards value 
    init (vcard[1], 255, 178, 280, 30); //comms cards value
/* touney info */
    init (tplace,   100, 461, 152, 16); //your place
    init (tpaidp,   100, 477, 101, 16); //paid positions
    init (tblind,   100, 493, 152, 16); //blinds
    init (tstave,   100, 525, 203, 16); //average and biggest stacks
/**/
    init (bfold,    395, 504, 0,0);     //button fold viewed, color  
  //  init (bfold,    417, 506,  50, 18); //button fold viewed, text 'Fold' 
    init (bcall,    532, 504, 0,0);     //button call/check viewed, color 
  //  init (bcall,    555, 495,  50, 18); //button call/check viewed, text 'Call' 
    init (check,    545, 506,  50, 18); //button check viewed, text 'Check'
    init (raise,    667, 504, 0,0);     //button raise viewed, color
    init (bback,    671, 478,  40, 18); //button i'm back, text 'Back'
    init (sftab,     13, 395, 0,0);     //presel fold to any bet viewed  
    init (pinfo,    205, 442, 0,0);     //page info viewed
    init (tinfo,     50, 493,  40, 16); //tourney info: text 'Blinds' 
    init (pchat,     35, 442, 0,0);     //page chat viewed
  //  init (pchatc,    67, 442, 0,0);     //cash: page chat 
  //  init (sfold,    417, 516, 0,0);     //presel fold
  //  init (scall,    544, 493, 0,0);     //presel call
  //  init (scany,    544, 517, 0,0);     //presel call any
  //  init (schec,    544, 469, 0,0);     //presel check
  //  init (sfoch,    417, 493, 0,0);     //presel fold/check
// this is for push.c -------------------------------------/
    init (ssonh,     13, 410, 0,0); //sitout next hand viewed
  //  init (ssonb,     13, 409, 0,0); //sitout next big blind
    init (vftab,     11, 424, 0,0); //slide view sftab viewed 
    init (hftab,     11, 373, 0,0); //slide hide sftab viewed
  //  init (pchap,     51, 446, 0,0); //page chat playmoney
    init (bocha,     30, 534, 0,0); //box of chat viewed
    init (bobet,    571, 468, 0,0); //box of bet viewed
    init (pbmin,    544, 441, 0,0); //preset bet min
    init (pb3bb,    612, 441, 0,0); //preset bet 3bb
    init (pbpot,    677, 441, 0,0); //preset bet pot
    init (pbmax,    747, 441, 0,0); //preset bet max
    init (sinfo,    346, 468, 0,0); //slide up bar in info page
//---------------------------------------------------------/
    init (ccard[0], 373, 342, 0,0); //holds cards color
    init (ccard[1], 420, 342, 0,0);   
    init (ccard[2], 290, 182, 0,0); //comms cards color
    init (ccard[3], 344, 182, 0,0);   
    init (ccard[4], 398, 182, 0,0);   
    init (ccard[5], 453, 182, 0,0);   
    init (ccard[6], 507, 182, 0,0);  
/* timebank bar */
    int j, x[]={345,355,365,375,385,396,406,416,426,436}, y=432;
    if (Seats(tb)==6) { for (j=0; j<10; j++) x[j] += 12; 	} else
    if (Seats(tb)==2) { for (j=0; j<10; j++) x[j] += 12; y-=15; }
    init (tmbnk[0], x[0], y, 0,0); //10%  
    init (tmbnk[1], x[1], y, 0,0);  
    init (tmbnk[2], x[2], y, 0,0);  
    init (tmbnk[3], x[3], y, 0,0);  
    init (tmbnk[4], x[4], y, 0,0);  
    init (tmbnk[5], x[5], y, 0,0);  
    init (tmbnk[6], x[6], y, 0,0);  
    init (tmbnk[7], x[7], y, 0,0);  
    init (tmbnk[8], x[8], y, 0,0);  
    init (tmbnk[9], x[9], y, 0,0); //100%
/**/
/* Player positions -------------------//-------------------------------------\
|               /(5)-(6)\              |               /--(4)--\              |
|        ...  (4)       (7)            |         ... (3)       (5)            |
|    player3 (3)         (8)           |              |         |             |
|    player2  (2)       (9) player9    |     player2 (2)       (6) player6    |
|               \--(1)--/              |               \--(1)--/              |
|             player1 (Me)             |             player1 (Me)             |
\--------------------------------------//------------------------------------*/
#define SW 90 //width, res:792x546
#define SH 20 //height 
#define BW 100
#define BH 16
// table of 9 seats ---------------------------------------/
  if (Seats(tb)==9)
  { init (stack[1],  329, 404, SW, SH); //seat1 stack,ident
    init (stack[2],  103, 362, SW, SH);
    init (stack[3],   11, 247, SW, SH);
    init (stack[4],   54, 134, SW, SH);
    init (stack[5],  210,  78, SW, SH);
    init (stack[6],  490,  78, SW, SH);
    init (stack[7],  646, 134, SW, SH);
    init (stack[8],  689, 247, SW, SH);
    init (stack[9],  598, 363, SW, SH); //seat9
    init (dealer[1], 459, 339,  0,  0); //dealer   
    init (dealer[2], 254, 328,  0,  0); 
    init (dealer[3], 148, 215,  0,  0);  
    init (dealer[4], 178, 176,  0,  0); 
    init (dealer[5], 283, 128,  0,  0);  
    init (dealer[6], 511, 128,  0,  0);  
    init (dealer[7], 606, 176,  0,  0);  
    init (dealer[8], 632, 211,  0,  0);  
    init (dealer[9], 542, 323,  0,  0); 	     
    init (betpr[1],  376, 300, BW, BH); //betpr
    init (betpr[2],  245, 287, BW, BH); 
    init (betpr[3],  176, 245, BW, BH); 
    init (betpr[4],  218, 143, BW, BH); 
    init (betpr[5],  321, 124, BW, BH); 
    init (betpr[6],  394, 113, BW, BH); 
    init (betpr[7],  476, 143, BW, BH); 
    init (betpr[8],  518, 248, BW, BH); 
    init (betpr[9],  455, 287, BW, BH); 
    init (empty[1],  318, 404,  0,  0); //empty
    init (empty[2],   92, 362,  0,  0);
    init (empty[3],    0, 247,  0,  0);
    init (empty[4],   43, 134,  0,  0);
    init (empty[5],  199,  78,  0,  0);
    init (empty[6],  590,  78,  0,  0);
    init (empty[7],  746, 134,  0,  0);
    init (empty[8],  789, 247,  0,  0);
    init (empty[9],  698, 363,  0,  0);
  }
// table of 6 seats ---------------------------------------/
  else if (Seats(tb)==6)
  { init (stack[1],  373, 403, SW, SH); //seat1 stack,ident
    init (stack[2],   33, 307, SW, SH); 
    init (stack[3],   33, 134, SW, SH); 
    init (stack[4],  330,  68, SW, SH); 
    init (stack[5],  672, 134, SW, SH); 
    init (stack[6],  672, 307, SW, SH); //seat6
    init (dealer[1], 458, 343,  0,  0); //dealer
    init (dealer[2], 198, 307,  0,  0);  
    init (dealer[3], 173, 175,  0,  0); 
    init (dealer[4], 332, 122,  0,  0); 
    init (dealer[5], 618, 182,  0,  0);  
    init (dealer[6], 584, 307,  0,  0);          
    init (betpr[1],  342, 299, BW, BH); //betpr
    init (betpr[2],  184, 270, BW, BH); 
    init (betpr[3],  206, 145, BW, BH);
    init (betpr[4],  370, 112, BW, BH);
    init (betpr[5],  494, 145, BW, BH);
    init (betpr[6],  508, 270, BW, BH); 
    init (empty[1],  472, 403,  0,  0); //empty
    init (empty[2],   22, 307,  0,  0); 
    init (empty[3],   22, 134,  0,  0);
    init (empty[4],  318,  68,  0,  0);
    init (empty[5],  772, 134,  0,  0);
    init (empty[6],  772, 307,  0,  0);
  }
// table of 2 seats ---------------------------------------/
  else if (Seats(tb)==2)
  { init (stack[1],  372, 389, SW, SH); //seat1 stack,ident
    init (stack[2],  330,  78, SW, SH); //seat2
    init (dealer[1], 473, 310,  0,  0); //dealer	
    init (dealer[2], 318, 132,  0,  0); 
    init (betpr[1],  342, 282, BW, BH); //betpr
    init (betpr[2],  369, 122, BW, BH); 
    init (empty[1],  320,  78,  0,  0); //empty
    init (empty[2],  473, 389,  0,  0);
    //correct hold cards place
    GMP(tb)->vcard[0].y -= 15;
    GMP(tb)->ccard[0].y -= 15;
    GMP(tb)->ccard[1].y -= 15;
  }
//---------------------------------------------------------/
    return;
}

/*****************************************************************************/
int gmtr_calc0 (psb_t *tb)
{   if (!tb) {error(0,0,"gmtr_calc0: null pointer"); return 0;}
    static int win_w, win_h;
    int ret=0, ww=Win_w(tb), wh=Win_h(tb);
    
    getwinsize(tb);
    
    if (win_w != ww || win_h != wh) //if table's window changes size
    {   Info(tb,"# calculating table's geometries for: %dx%d\n", ww, wh);
	
	gmtr_init(tb);
	//initialize idents geometries-/
	int j;
	gmtx_t gm;
	for (j=1; j<=Seats(tb); j++)
        {   gm = GMP(tb)->stack[j];
	    gm.y -= gm.h;
	    GMP(tb)->ident[j] = gm;
	}
	//-----------------------------/
/*	
	gmtx_t *gp=(gmtx_t*)GMP(tb);
	int j, n=(sizeof(gmtr_t)/sizeof(gmtx_t)); //total geometries in gmtr_t
	for (j=0; j<n; j++,gp++)
	{   gp->x = gp.fg.x * Win_w(tb);
	    gp->y = gp.fg.y * Win_h(tb);
	    gp->w = gp.fg.w * Win_w(tb);
	    gp->h = gp.fg.h * Win_h(tb);
	}
*/
	win_w = ww;
	win_h = wh;
	ret=1;
    }
    if (Debug(tb)) capt_gmtr0(tb);

    return ret;
}

/*****************************************************************************/

