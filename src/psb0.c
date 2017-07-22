
#include <xdo.h>
#include <sys/stat.h>  //
#include <sys/types.h> //mkdir()
#include <math.h>
#include "util.h"
#include "psb0.h"

//local function declarations-------------------------------------------------/
//static val_t valuecard (char ch);
static inline void assignvars (psb_t *tb); //
static inline void assigncrds (psb_t *tb); // just to organize a little
static inline void assignevls (psb_t *tb); //
static inline pos_t findposit (psb_t *tb); //
static inline pos_t getposits (psb_t *tb); //player posit, 
static inline void  getstates (psb_t *tb); //player stack,state,betpr
static inline void  getsawfls (psb_t *tb); //player sawfl,round
static inline void  getcounts (psb_t *tb); //state player counters
static inline int   getsinbbs (psb_t *tb); //sinbb - set sizes in bbs
static inline int  save_logtb (psb_t *tb); //save out and capt of table in logdir
static void  getidents (psb_t *tb); //player ident 
static stg_t findstage (psb_t *tb, stgi_t *ps); //find stage, ps: prev/next info  
static int   getblinds (psb_t *tb); //try to get blinds from table
static int   crttmpdir (psb_t *tb, char *ret); //create unique dir, ret dirname
static int   createdir (psb_t *tb, char *dir); //create directory named dir
static int   printout0 (psb_t *tb, FILE *fp); //print output to file

//global function declarations------------------------------------------------/
psb_t* psb_psb0 (void); //create a useable psb_t struct 
int    psb_free (psb_t *tb);
int    psb_init (psb_t *tb, Window id, int sits); //init table's stuffs-------/
int    psb_loop (psb_t *tb, int delay); //wait turn, capt and show. return 1 if  
int    psb_capt (psb_t *tb); //capture all data		//its your turn to play
int    psb_show (psb_t *tb); //print to stdout/logout	//--------------------/
int    psb_turn (psb_t *tb); //you turn to play
int    psb_wait (psb_t *tb, int perc); //wait percent of timebank
Window parsewid (char  *st); //get table's window id from string argument
Window byselwid (psb_t *tb); //get table's window id selecting with mouse 
int    psb_actw (psb_t *tb, int delay); //activate table's window and sleep
int    psb_args (psb_t *tb, int argc, char **argv); //parse args, init psb_t vars
int    psb_inpp (psb_t *tb, pos_t ra, pos_t po); //preflop position
int    psb_prpo (psb_t *tb, pos_t ra, pos_t po); //fast, need psb_capt() before
void   psb_info (psb_t *tb, const char *fmt, ...); //show info to log/out

int  getwinsize (psb_t *tb); //size of window, ajust h/w relation
int  setwinsize (psb_t *tb, unsigned int w, unsigned int h);
int  sort_cards (card_t *cs, int len, int cmp); //sort cards, cmp:0 low to high
//------------------------------------/
sit_t  pos2sit  (psb_t *tb, pos_t po);
pos_t  sit2pos  (psb_t *tb, sit_t se);
char*  pos2str  (pos_t x);
char*  stg2str  (stg_t x);
char*  sta2str  (sta_t x);
char*  col2str  (col_t x);
char*  val2str  (val_t x);
char*  rnk2str  (rnk_t x);
char*  evl2str  (rnk_t x);
char*  psh2str  (psh_t x);
pos_t  str2pos  (char *s); 

//orders to talk pre and post flop--------------------------------------------/
#define BU PSBPOS_BU,
#define CO PSBPOS_CO,
#define M1 PSBPOS_M1,
#define M2 PSBPOS_M2,
#define M3 PSBPOS_M3,
#define M4 PSBPOS_M4,
#define U1 PSBPOS_U1,
#define UT PSBPOS_UT,
#define SB PSBPOS_SB,
#define BB PSBPOS_BB,
pos_t prefOrder[11][11]= //[seats-emptys][posits] 
{	{0}, {0}, 
	{0, BU BB			  }, //2* 
	{0, BU SB BB 			  }, //3
	{0, UT BU SB BB 		  }, //4
	{0, UT CO BU SB BB 		  }, //5
	{0, UT U1 CO BU SB BB 		  }, //6*
	{0, UT U1 M1 CO BU SB BB 	  }, //7
	{0, UT U1 M2 M1 CO BU SB BB 	  }, //8
    	{0, UT U1 M3 M2 M1 CO BU SB BB 	  }, //9*
	{0, UT U1 M4 M3 M2 M1 CO BU SB BB }, //10
};

pos_t posfOrder[11][11]= //[seats-emptys][posits] 
{      //0   1  2  3  4  5  6  7  8  9 10       
        {0}, {0},
	{0, BB BU			  }, //2*
	{0, SB BB BU 			  }, //3
	{0, SB BB UT BU			  }, //4
	{0, SB BB UT CO BU		  }, //5
	{0, SB BB UT U1 CO BU		  }, //6*
	{0, SB BB UT U1 M1 CO BU	  }, //7
	{0, SB BB UT U1 M2 M1 CO BU	  }, //8
    	{0, SB BB UT U1 M3 M2 M1 CO BU    }, //9*
	{0, SB BB UT U1 M4 M3 M2 M1 CO BU }, //10
};
#undef BU 
#undef CO 
#undef M1 
#undef M2 
#undef M3 
#undef M4 
#undef U1 
#undef UT 
#undef SB 
#undef BB 

//----------------------------------------------------------------------------/
Window parsewid (char *sarg) 
{   Window wi=0;
    if (!sarg) error(EXIT_FAILURE,0,"invalid window id string");
    sscanf(sarg, "0x%lx", &wi);
    if ( !wi ) sscanf(sarg, "%lu", &wi);
    //if ( !wi ) error(EXIT_FAILURE,0,"invalid window id format");
    return wi;
}

Window byselwid (psb_t *tb)
{   if (!tb) {error(0,0,"byselwid: null pointer"); return 0;}
    Window ret;
    Info(tb,"# please, select the table with the mouse:\n");	
    xdo_select_window_with_click(XTP(tb), &ret);
    return ret;
}

#define GETWINSIZE0 2
#define WINSIZERES0 0.6894
int getwinsize (psb_t *tb)
{   //if (!tb) {error(0,0,"getwinsize: null pointer"); return 0;}
    unsigned int ret=1, w, h;
    float res=WINSIZERES0, tol=0.001, hwr=0;
    
    xdo_get_window_size(XTP(tb), Winid(tb), &w, &h);
    hwr = (float) h/w;
    if (Debug(tb)) fprintf(stderr,"getwinsize: w=%d h=%d hwr=%.4f\n",w,h,hwr);
    if (hwr <= (res-tol) || hwr >= (res+tol))
    {   xdo_set_window_size(XTP(tb), Winid(tb), w, h=roundf(res*w), 0);
	sleep(GETWINSIZE0);
	ret = 0;
	hwr = (float) h/w;
	Info(tb,"# ajusting h/w window relation: %4f\n", hwr);
    if (Debug(tb)) fprintf(stderr,"getwinsize: w=%d h=%d hwr=%.4f\n",w,h,hwr);
    }
    Win_h(tb) = h;
    Win_w(tb) = w;
    //Info(tb,"# getting size of table's window: %dx%d\n", w, h);
/*
    XWindowAttributes wa;
    XGetWindowAttributes(tb->cf.dpy, Winid(tb), &wa);
    tb->cf.win_w = wa.width;
    tb->cf.win_h = wa.height; 
*/
    return ret;
}

#define SETWINSIZE0 2
int setwinsize (psb_t *tb, unsigned int w, unsigned int h)
{   //if (!tb) {error(0,0,"setwinsize: null pointer"); return 0;}
    int ret = xdo_set_window_size(XTP(tb), Winid(tb), w, h, 0);
    sleep(SETWINSIZE0);
    Win_h(tb) = h;
    Win_w(tb) = w;
    Info(tb,"# setting size of table's window to: %dx%d\n", w, h);
    getwinsize(tb);
    return ret;
}

int psb_actw (psb_t *tb, int delay)
{   if (!tb) error(EXIT_FAILURE,0,"psb_actw: null pointer");
    //needs window manager support
    xdo_activate_window(XTP(tb), Winid(tb));
    xdo_wait_for_window_active(XTP(tb), Winid(tb), 1);
    /*
    //needs raised windows
    xdo_raise_window(XTP(tb), Winid(tb));
    xdo_focus_window(XTP(tb), Winid(tb));
    xdo_wait_for_window_focus(XTP(tb), Winid(tb), 1);
    */
    sleep(delay);
    return 1;
}
void* xmalloc (size_t size)
{   void *ptr = malloc (size);
    if (!ptr) error(EXIT_FAILURE,0,"virtual memory exhausted");
    memset(ptr,0,size);
    return ptr;
}

psb_t* psb_psb0 (void) 
{   psb_t *tb = (psb_t *)xmalloc(sizeof(psb_t));
    tb->cf.gm = xmalloc(sizeof(gmtr_t));
    tb->cf.ca = xmalloc(sizeof(capt_t));
    tb->cf.ev = xmalloc(sizeof(eval_t));   
/*
    psb_t *tb = malloc(sizeof(psb_t)), tb0={0}; *tb = tb0;
    tb->cf.gm = malloc(sizeof(gmtr_t)); gmtr_t gs0={0}; *(GMP(tb))=gs0;
    tb->cf.ca = malloc(sizeof(capt_t)); capt_t cp0={0}; *(CAP(tb))=cp0;
    tb->cf.ev = malloc(sizeof(eval_t)); eval_t ev0={0}; *(EVP(tb))=ev0;
*/
    tb->cf.dbgdir = xmalloc(80*sizeof(char)); tb->cf.dbgdir[80] = '\0';
    tb->cf.logdir = xmalloc(80*sizeof(char)); tb->cf.logdir[80] = '\0';
    return tb;
}    

int psb_init (psb_t *tb, Window id, int sits) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_init: null pointer");
    Seats(tb) = (!sits)? tb->cf.seats : sits;
    Winid(tb) = ( !id )? tb->cf.win   : id;
    if (!Seats(tb)) Seats(tb) = 9; //default number of seats

    if (Debug(tb)) 
    {   createdir(tb,"/tmp/psbot0-debug");
	strncpy(tb->cf.dbgdir,"/tmp/psbot0-debug",80);
    }
    if (tb->cf.logtb) 
    {   if ( !crttmpdir(tb,tb->cf.logdir) )
	    Quit(tb,10,"# exit: couldn't create log directory\n");
        else
        {   Info(tb,"# logging to directory: %s\n", tb->cf.logdir);  
      	    char s0[48];
            sprintf(s0, "%s/log0.dat", tb->cf.logdir); 
            errno = 0;
	if ( !(tb->cf.logout = fopen(s0, "a")) )	
	  Quit(tb,11,"# exit: couldn't create log file: %s\n",strerror(errno));
        }
    } 
    
    Info(tb,"> initializing psbot0 data...\n");
    if ( !(Seats(tb)==9 || Seats(tb)==6 || Seats(tb)==2) )
    	Quit(tb,12,"# exit: invalid number of seats!\n"); 
    if ( !(tb->cf.dpy = XOpenDisplay( getenv("DISPLAY") )) )
	Quit(tb,13,"# exit: cannot connect to X server.\n");
    if ( !(tb->cf.xt = xdo_new_with_opened_display(tb->cf.dpy, 0, 0)) )
	Quit(tb,14,"# exit: failed to create xdo object.\n");	
       
    if (!Winid(tb)) Winid(tb) = byselwid(tb);
    switch (tb->cf.wsize)
    {   case  0: setwinsize(tb, 792, 546); break;
        case -1: getwinsize(tb);           break;
        default: setwinsize(tb, tb->cf.wsize, tb->cf.wsize*WINSIZERES0); break;
    }
    
    gmtr_calc0(tb); //reset geometries
    psb_actw(tb,2); 
    capt_capt0(tb); //capture some vars
    psb_push(tb,PSB_BUTIBACK); sleep(1);
    psb_push(tb,PSB_PAGEINFO); sleep(1);
    
    if (tb->bl_sb<PSBZERO || tb->bl_bb<PSBZERO) 
    {   Info(tb,"# blinds: not setted, trying to find them...\n");
        if (CAP(tb)->tinfo) 
	    Info(tb,"# blinds: found! they're in tourney info.\n");
        else if (getblinds(tb)) 
	    Info(tb,"# blinds: found!\n");
        else 
	    Quit(tb,15,"# blinds: failed! try again or set them by hand.\n");
    }
    if (tb->bl_sb>PSBZERO && tb->bl_bb>PSBZERO) 
        Info(tb,"# blinds setted to: sb=%.2f, bb=%.2f,\n",tb->bl_sb,tb->bl_bb);

    Info(tb,"# getting ident of players...\n");
    getidents(tb);

    Info(tb,"< end of init, now to play!\n");
    return 1;
}

int psb_free (psb_t *tb) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_free: null pointer"); 
/*
    if (tb->cf.logdir)
    {   char rm[32];
        strncpy (rm, "rm -rf ", 8);
        strncat (rm, tb->cf.logdir, strlen(tb->cf.logdir)+1);
	if (system(rm) == -1) 
	    error(EXIT_FAILURE,0,"psb_free: couldn't remove tmp dir.");
    }
*/
    XCloseDisplay( tb->cf.dpy );
    if (tb->cf.dbgdir) free(tb->cf.dbgdir);
    if (tb->cf.logdir) free(tb->cf.logdir);
    if (tb->cf.logout) fclose(tb->cf.logout);
    xdo_free(XTP(tb));
    free(GMP(tb));
    free(CAP(tb));
    free(EVP(tb));
    free(tb);
    return 1;
}

int psb_wait (psb_t *tb, int perc) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_wait: null pointer");
    int j=20; //max time 20seconds    
    if (perc<=0 || perc>100) return 1;
    Info(tb,"# waiting %d%% of banktime\n", perc);
    while (j) 
    {   if (capt_captt(tb) <= perc) break; else j--; 
	fprintf(stdout,"."); fflush(stdout);
	sleep(1); 
    }
    fprintf(stdout,"\n"); fflush(stdout);
    return 1;
}

int psb_prpo (psb_t *tb, pos_t range, pos_t posit)
{   if (!tb) error(EXIT_FAILURE,0,"psb_prpo: null pointer");
    int r=0, p=0;
    if (range) 
    {   r = (range >= tb->posit); 
        Info(tb,"# pp_in: %sin range:%s\n",(r)?"":"not ",pos2str(range));
    }
    if (posit)
    {   p = (posit == tb->posit);
        Info(tb,"# pp_in: %sin posit:%s\n",(p)?"":"not ",pos2str(posit));
    }
    return (tb->pp_in = r||p);
}

int psb_inpp (psb_t *tb, pos_t range, pos_t posit)
{   if (!tb) error(EXIT_FAILURE,0,"psb_inpp: null pointer");
    tb->pp_in = 0;
    psb_capt(tb);
    if (tb->stage!=PSBSTG_PR) Info(tb,"# pref pos: not in preflop stage.\n"); 
    else if (!tb->posit)      Info(tb,"# pref pos: unknown player posit.\n");
    else 
	tb->pp_in = psb_prpo(tb,range,posit); 	
    return tb->pp_in;
}

pos_t sit2pos (psb_t *tb, sit_t se)
{   if (se<PSBSITxx || se>Seats(tb)) 
    {   Info(tb,"# sit2pos: attention here, invalid seat.");
 	return PSBPOS_xx;
    }
    else return tb->cf.topos[ se ];
}

sit_t pos2sit (psb_t *tb, pos_t po)
{   if (po<PSBPOS_xx || po>PSBPOS_UT)
    {   Info(tb,"#pos2sit: attention here, invalid posit.\n");
        return PSBSITxx;
    }
    else return tb->cf.tosit[ po ];
}

static int getblinds (psb_t *tb)
{   if (!tb) return 0;
    sit_t s, se=Seats(tb);
    float x[10]={0}, ax;
    int n=0, j, k, max=60; //max time 2minute
    stgi_t ps={0}; //previous stage info

    while (max) 
	if (findstage(tb,&ps) <= PSBSTG_PR) break; 
	else { sleep(2); max--; fprintf(stdout,"."); }
    sleep(4); 
    fprintf(stdout,"\n");
    if (max)
    { capt_captb(tb);
      for (s=1; s<=se; s++) 
	  if (CAP(tb)->betpr[s] > PSBZERO) x[n++]=CAP(tb)->betpr[s];
      for (j=0; j<n-1; j++) for (k=j+1; k<n; k++) 
	  if (x[j] > x[k]) { ax = x[j]; x[j] = x[k]; x[k] = ax; }
      tb->bl_sb = (x[0]>PSBZERO)? x[0] : 0;
      tb->bl_bb = (x[1]>PSBZERO)? x[1] : 0;
      if ((x[1]>PSBZERO) && (x[0]==x[1])) tb->bl_sb=x[1]/2; 
    }
    return (tb->bl_sb>PSBZERO && tb->bl_bb>PSBZERO);
}

static int crttmpdir (psb_t *tb, char *ret)
{   char xdir[] = "/tmp/psbot0-XXXXXX";
    if (mkdtemp(xdir)) strncpy(ret, xdir, 80);
    return (ret)? 1: 0;
}

static int createdir (psb_t *tb, char *dir)
{   errno = 0;
    mkdir(dir, 0700); 
    switch (errno)
    { case EEXIST: break;
      case -1: Quit(tb,5,"# exit: couldn't create dir: %s\n", strerror(errno));
	       break;
      default: Info(tb,  "# creating directory: %s.\n",dir); break;
    }
    return 0;
}

int psb_turn (psb_t *tb) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_turn: null pointer"); 
    if ( !capt_captu(tb) ) fprintf(stdout,"."); else fprintf(stdout,"\n");
    fflush(stdout);
    return CAP(tb)->yturn;
}

int psb_loop (psb_t *tb, int delay) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_loop: null pointer"); 
    stgi_t sg={0}; //previous stage info
    int ret=1;

    psb_actw (tb,delay);
    while (1)
    {   if ((ret=psb_turn(tb)) || (findstage(tb,&sg)==PSBSTG_FL && !sg.co)) 
        {   psb_capt(tb); 
            psb_show(tb);
            if (ret) break;
        }    
        psb_actw (tb,delay);
    }
    return ret;
}

//----------------------------------------------------------------------------/
static inline void assignvars (psb_t *tb)
{   tb->torse = (CAP(tb)->raise)? CAP(tb)->torse: 0;
    tb->tocll = 
      (CAP(tb)->bcall)? CAP(tb)->tocll : ((!CAP(tb)->check)? CAP(tb)->torse:0);
    tb->check = CAP(tb)->check;
    tb->bfold = CAP(tb)->bfold;
    tb->bcall = CAP(tb)->bcall;
    tb->pot_t = CAP(tb)->pot_t;
    tb->pot_s = CAP(tb)->pot_s;
    tb->bback = CAP(tb)->bback;
    tb->sftab = CAP(tb)->sftab;
    tb->tinfo = CAP(tb)->tinfo;
    tb->yturn = CAP(tb)->yturn;
    
    tinf_t ti0={0};
    if (CAP(tb)->tinfo)
    {   ti0.bl_bb = CAP(tb)->tblind[0];			
	ti0.bl_sb = CAP(tb)->tblind[1]; 
	if (ti0.bl_bb < tb->bl_bb) ti0.bl_bb = tb->bl_bb; //------------------/
	if (ti0.bl_sb < tb->bl_sb) ti0.bl_sb = tb->bl_sb; //fix if wrong capture 
	ti0.antes = CAP(tb)->tblind[2];	
	ti0.place = CAP(tb)->tplace;
	ti0.total = CAP(tb)->ttotal;
	ti0.paidp = CAP(tb)->tpaidp;
	ti0.stave = CAP(tb)->tstave;
	ti0.stbig = CAP(tb)->tstbig;
    }
    tb->ti = ti0;
    tb->bl_bb = (tb->ti.bl_bb)? tb->ti.bl_bb : tb->bl_bb;
    tb->bl_sb = (tb->ti.bl_sb)? tb->ti.bl_sb : tb->bl_sb;
    tb->antes = (tb->ti.antes)? tb->ti.antes : tb->antes;
    return;
}

static inline void assigncrds (psb_t *tb)
{   int j;
    for (j=0;j<7;j++) 
    {   tb->cards[j].v = (CAP(tb)->ccard[j])? CAP(tb)->vcard[j] : 0; 
	tb->cards[j].c = (CAP(tb)->vcard[j])? CAP(tb)->ccard[j] : 0; 
    }
    for (j=0; j<2; j++) tb->holds[j] = tb->cards[j]; 
    for (j=0; j<5; j++) tb->comms[j] = tb->cards[j+2]; 
    for (j=0; j<7; j++) tb->sdrac[j] = tb->cards[j]; 
    sort_cards(tb->holds, 2,0);
    sort_cards(tb->comms, 5,0);
    sort_cards(tb->sdrac, 7,0);
    return;
}

static inline void assignevls (psb_t *tb)
{   int j;
    eval_eval0(tb);
    tb->hrank = EVP(tb)->hrank;
    tb->heval = EVP(tb)->heval;
    for (j=0; j<5; j++) tb->hbest[j] = EVP(tb)->bhand[j];
    tb->rdraw = 0;
    tb->edraw = 0;
    if (EVP(tb)->fldrw == 1) 
    {   tb->rdraw = PSBRNK_FL; 
	tb->edraw = EVP(tb)->flevl;
    }
    else 
    if (EVP(tb)->stdrw == 1)
    {   tb->rdraw = PSBRNK_ST; 
	tb->edraw = EVP(tb)->stevl;
    }
    return;
}
/*
static val_t valuecard (char ch)
{   switch (ch)
    {   case ' ': return 0;
	case '1': return PSBVT; 
	case 'J': return PSBVJ;
	case 'Q': return PSBVQ;
	case 'K': return PSBVK; 
	case 'A': return PSBVA;
	default:  return ch-'0';
    }
}
*/
int cmp_flt (const void *a, const void *b)
{   const float *da=(const float *)a, *db=(const float *)b;
    return (*da > *db) - (*da < *db);
}
int cmp_int2l (const void *v1, const void *v2) //high to low
{   const card_t *c1=v1, *c2=v2; 
    return ( (c1->v == c2->v)? 0 : ((c1->v > c2->v)? 1 : -1) ); 
}
int cmp_int2h (const void *v1, const void *v2) //low to high
{   const card_t *c1=v1, *c2=v2; 
    return ( (c1->v == c2->v)? 0 : ((c1->v > c2->v)? -1 : 1) ); 
}
int sort_cards (card_t *cs, int len, int cmp) 
{   int j=0;
    if (!cs || len<=0) return 0;
    for (j=0; j<len; j++) if (!cs[j].v) {len=j; break;} //no zero cards
    qsort (cs, len, sizeof(card_t), (cmp)? cmp_int2h : cmp_int2l);
    return 1;
}

static inline pos_t findposit (psb_t *tb) //dep: getstates() findstage()
{   static time_t lst; time_t now;//rndtm //set: dealer,posit,nwrnd,kepts,round
    int j, rndtm=0;
    static sit_t dealer; //previous dealer
    
    tb->nwrnd = ((tb->cf.dealer = CAP(tb)->dealer) != dealer);
    if (tb->cf.dealer && tb->nwrnd)
    {   tb->cf.kepts = Seats(tb) - tb->empty;
	tb->posit = getposits(tb);
    }
    if (tb->nwrnd) 
    {   if (!tb->round) lst = time(0);  
	now=time(0); rndtm=(now-lst); lst=now; tb->rndtm = (tb->rndtm+rndtm)/2;
	tb->round += 1; 
	for (j=1; j<=Seats(tb); j++) 
	    if (tb->se[j].state != PSBSTA_SEMPTY) tb->se[j].round += 1;
    }
    if (Debug(tb))
    { fprintf(stderr,"psb0: findposit: de=%d,%d po=%s nw=%d rn=%d tm=%d(%d)\n",
       tb->cf.dealer, dealer, pos2str(tb->posit), tb->nwrnd, tb->round, (int)
       tb->rndtm, (int)rndtm);
    }
    dealer = tb->cf.dealer;
    return tb->posit;
}

static stg_t findstage (psb_t *tb, stgi_t *sg) 
{   int j, cs[]={0,1,4,5,6}; 
    stg_t prev_stage = sg->stage;
    noy_t nwrnd=0;

    capt_stage(tb);
    nwrnd = (CAP(tb)->dealer != tb->cf.dealer);
    for (j=4; j>=0; j--) if (CAP(tb)->ccard[ cs[j] ]) {sg->stage=j; break;}
    if (sg->stage) 
    {   sg->nw = ((sg->stage != prev_stage) || nwrnd);
	if (sg->nw) sg->co=0; else sg->co++; 
    }
    if (Debug(tb))
    {   fprintf(stderr,"psb0: findstage: sg=%s ps=%s nw=%d co=%d j=%d\n", 
	stg2str(sg->stage), stg2str(prev_stage), sg->nw, sg->co, j);	
    }
    return sg->stage;
}

static void getidents (psb_t *tb) 	//dep: seats
{   int j;				//set: ident
    char *ip=0;
#define notIdents /*--------------------*/\
(   !strncmp (ip,"Fold", 4) || \
    !strncmp (ip,"Call", 4) || \
    !strncmp (ip,"Raise",5) || \
    !strncmp (ip,"Bel",  3) || /*Bet*/\
    !strncmp (ip,"Check",5) || \
    !strncmp (ip,"PoslS",5) || /*Post SB*/\
    !strncmp (ip,"PoslB",5) || /*Post BB*/\
    !strncmp (ip,"Anle" ,4) || /*Ante*/\
    !strncmp (ip,"Time" ,4)    /*from second timebank*/\
)/*-------------------------------------*/
    capt_state(tb);
    for (j=1; j<=Seats(tb); j++)
    {   ip = CAP(tb)->ident[j];
	if (CAP(tb)->empty[j]) tb->se[j].ident[0] = '\0'; else 
	    if ( !notIdents ) strncpy(tb->se[j].ident, ip, 5); 
	if (Debug(tb))
	{   fprintf(stderr,"psb0: getidents: sit=%d ide=%s, emp=%d *ip=%.*s,\n", 
	    j, tb->se[j].ident, CAP(tb)->empty[j], 5, ip); 
        }
    }
    return;
}

static inline void getstates (psb_t *tb) //dep: seats,stage
{   int j=0;				 //set: stack,state,betpr,ident,empty
    sta_t state=0;
    char *sp=0;
    noy_t preflop=(tb->stage==PSBSTG_PR);
    char *ip=0;
    seat_t s0={0};

    capt_state(tb);
    if (preflop) capt_captb(tb);
    tb->empty = 0;
    for (j=1; j<=Seats(tb); j++) 
    {   sp = CAP(tb)->stack[j]; //---------------------------stack------------/
        if (CAP(tb)->empty[j]) 
        {   state = PSBSTA_SEMPTY; 
	    tb->se[j] = s0;
	    tb->empty++; 
        } else
        if (sp[0] == '\0' )  state = PSBSTA_FOLDOUT; 	      	       else
        if (strstr(sp,"A")) {state = PSBSTA_ALLIN; tb->se[j].stack=0;} else
        if (strstr(sp,"n"))  state = PSBSTA_SITOUT;  
          else 
	{   state = PSBSTA_PLAYING; 
	    tb->se[j].stack = atof(sp); 
	}
        tb->se[j].state = state; //--------------------------state------------/
	if (preflop) tb->se[j].betpr = CAP(tb)->betpr[j]; //-betpr------------/
        ip = CAP(tb)->ident[j];  //--------------------------ident------------/
	if (tb->yturn && !CAP(tb)->empty[j] && 
	   (ip[0] != '\0') && strncmp(ip,tb->se[j].ident,5) && !notIdents)
	{   strncpy(tb->se[j].ident, ip, 5); 
	    tb->se[j].round = tb->se[j].sfcnt = tb->se[j].sawfl = 0;
	}    
	if (Debug(tb)) 
        {   fprintf(stderr,"psb0: getstates: seat=%d, state=%s, stack=%.2f," 
             " betpr=%.2f, ", j, sta2str(tb->se[j].state), tb->se[j].stack, 
             tb->se[j].betpr);
            fprintf(stderr,"ident=%s, cp.ident=%.*s, sp=%s,\n", tb->se[j].ident, 
             5,CAP(tb)->ident[j], sp); 
        }
    }
    tb->mystk = tb->se[ PSBSIT1 ].stack;
#undef notIdents
    return;
}
					  //dep: seats,kepts,dealer,state,betpr
static inline pos_t getposits (psb_t *tb) //set: topos,tosit
{   sit_t se=Seats(tb), *ks=&tb->cf.kepts, seat, sit_order[11]={0};
    int s, j=1, empty, isout;
    
    for (s=1; s<=se; s++) sit_order[s] = s;
    rotarray((int*)(sit_order+1), se, se-tb->cf.dealer); 
    for (s=1; s<=se; s++) 
    {   seat = sit_order[s];
        tb->cf.topos[seat] = 0;
	empty = (tb->se[seat].state == PSBSTA_SEMPTY);
	isout = (tb->se[seat].state == PSBSTA_FOLDOUT);
	if (isout && j<=2 && !tb->se[seat].betpr) (*ks)--; 
	else tb->cf.topos[seat] = (!empty)? posfOrder[*ks][j++] : 0;  
	tb->se[seat].posit = tb->cf.topos[seat];
	if (Debug(tb)) 
        { fprintf(stderr,"psb0: getposits: s=%d j=%d seat=%d empty=%d isout=%d"
          " pos=%s,\n", s, j, seat, empty, isout, pos2str(tb->cf.topos[seat])); 
        }
    }
    for (s=1; s<=se; s++) tb->cf.tosit[ tb->cf.topos[s] ] = s; //remember seat
    if (Debug(tb)) 
    {   fprintf(stderr, "psb0: getposits: sit_order=");
        for (s=1; s<=se; s++) fprintf(stderr, "%d,", sit_order[s]); 
        fprintf(stderr, "\n");
    }
    return (tb->posit = tb->se[PSBSIT1].posit); 
}

static inline void getcounts (psb_t *tb) //dep: seats,kepts,stage
{   sit_t s, me=0, sit;			 //set: pls** 
    pos_t pos=0;
    sta_t sta=0;
    sit_t ks=tb->cf.kepts;
    pos_t *pos_order = (tb->stage==PSBSTG_PR)? prefOrder[ks] : posfOrder[ks];
        
    tb->plsin = tb->plsnt = tb->plsyt = 0;
    tb->plsfo = tb->plsai = tb->plsso = 0;
    for (s=1; s<=Seats(tb); s++)
    {   pos = pos_order[s];
	sit = pos2sit(tb, pos);
	sta = tb->se[sit].state;
	if (pos == tb->posit) me++; 
	else switch (sta)
	{   case PSBSTA_FOLDOUT: tb->plsfo++; break;
	    case PSBSTA_SITOUT : tb->plsso++; break;
	    case PSBSTA_ALLIN  : tb->plsai++; 
	    case PSBSTA_PLAYING: (me)? tb->plsnt++:tb->plsyt++; break;
	    default : ;
	}	
        if (Debug(tb)) 
        { fprintf(stderr,"psb0: getcounts: pos=%s sit=%d sta=%s me=%d fo=%d "
	   "so=%d ai=%d ia=%d ib=%d\n", pos2str(pos), sit, sta2str(sta), me, 
	   tb->plsfo, tb->plsso, tb->plsai, tb->plsnt, tb->plsyt); 
        }
    }
    tb->plsin = tb->plsnt + tb->plsyt;
    return;
}

static void getsawfls (psb_t *tb)	//dep: seats,state
{   int j;				//set: sfcnt,sawfl
    for (j=1; j<=Seats(tb); j++) 
    { if (!tb->se[j].round) tb->se[j].round++;
      if (tb->se[j].state==PSBSTA_ALLIN || tb->se[j].state==PSBSTA_PLAYING)
          tb->se[j].sfcnt++;
      tb->se[j].sawfl = ((float) tb->se[j].sfcnt / tb->se[j].round) * 100;
      if (Debug(tb))
      fprintf(stderr,"psb0: getsawfls: sit=%d, sta:%s rnd=%d sfc=%d sfp=%3d\n",
      j,sta2str(tb->se[j].state),tb->se[j].round,tb->se[j].sfcnt,tb->se[j].sawfl);
    }
    return;
}

static inline int getsinbbs (psb_t *tb)
{   int j;
    sta_t state;
#define bb(x) tb->x /= tb->bl_bb
    if (tb->cf.sinbb)
    {   if (!tb->bl_bb) Quit(tb,6,"# sinbb: no big blind to set amounts.\n");
	bb (torse);
	bb (tocll);
	bb (pot_t);
        bb (pot_s);
	bb (mystk);
        for (j=1; j<=Seats(tb); j++) 
	{   state = tb->se[j].state;
	    if (state==PSBSTA_PLAYING || state==PSBSTA_ALLIN)
	    {   bb (se[j].stack); 
		bb (se[j].betpr);
	    }
	} 
	bb (ti.stave);
	bb (ti.stbig);
#undef bb
    }
    return 1;
}

//save output and capture of table in logdir
static inline int save_logtb (psb_t *tb)
{   if (tb->cf.logtb)
    {   char s0[48];
        static int n;
    	sprintf(s0, "capt-%03d", n++);
        gmtx_t gw={s0, {}, 0, 0, Win_w(tb), Win_h(tb)};
        capt_save0(tb, &gw, 0, 0); 
        Info(tb,"# saving capt in: %s/%s.png\n", tb->cf.logdir, s0);
	printout0(tb, tb->cf.logout);
    }
    return 1;
}

//----------------------------------------------------------------------------/
int psb_capt (psb_t *tb)
{   if (!tb) error(EXIT_FAILURE,0,"psb_capt: null pointer"); 
    //int j;
    sit_t se=Seats(tb);
    clock_t tmf=0, tmi=0; //captm
    
    if (Debug(tb)) createdir(tb,tb->cf.dbgdir);
    gmtr_calc0(tb);
    tmi = clock();
    capt_capt0(tb);
    
    assignvars(tb); //assign some vars----------------/
    getstates(tb);  //se (state stack ident betpr empty)-/
        
    //stage nwstg spins stg-----------/
    if ( !(tb->stage = findstage(tb,&tb->sg)) ) 
	Info(tb,"# stage: unknown stage in table.\n");

    //posit round nwrnd rndtm se(round)-/
    if ( !findposit(tb) ) 
	Info(tb,"# posit: failed to search dealer.\n");

    //se (sfcnt sawfl)----------------/
    if (tb->stage==PSBSTG_FL && !tb->sg.co) 
    {   Info(tb,"# sawfl: watching the flop stage.\n");
        getsawfls(tb);
    }
       
    //pls**---------------------------/ 
    if (!tb->posit || !tb->stage)
    {   Info(tb,"# plsin: unknown posit/stage.\n");
        tb->plsin = tb->plsnt = tb->plsyt = se-1;
    }
    else getcounts(tb); 
    
    //rsepr cllpr lastp---------------/
    tb->rsepr = tb->cllpr = tb->lastp = 0;
    if (tb->stage == PSBSTG_PR)
    {   if (!tb->bl_bb) Info(tb,"# rsepr: unknown big blind.\n");
	else if (tb->posit == PSBPOS_BB) 
	     tb->rsepr = !tb->check;
        else tb->rsepr = (tb->tocll > tb->bl_bb)||(tb->sg.co && !tb->check);
	tb->cllpr = (tb->rsepr || tb->plsyt); 
    }
    tb->lastp = !tb->plsnt; 
        
    assigncrds(tb); //cards sdrac holds comms---------/
    assignevls(tb); //hrank heval hbest rdraw edraw---/

    //--------------------------------/
    tb->pp_in = 0;
    if (tb->stage == PSBSTG_PR) 
    {   if (!tb->posit) Info(tb,"# pp_in: unknown posit.\n");
        else		tb->pp_in = psb_prpo(tb, tb->pp_ra, tb->pp_po);	   	
    }

    getsinbbs(tb);

    tmf = clock();
    tb->cf.captm = ((float)(tmf-tmi)/1000000.0F)*1000; 
    save_logtb(tb);

    psb_wait(tb, tb->tmbnk);

    return (tb->yturn);
}

//functions for print info to stdout/file-------------------------------------/
char* pos2str (pos_t x)
{   if (x<PSBPOS_BB || x>PSBPOS_UT) x = 0;
    char *posits[] = {"","BB","SB","BU","CO","M1","M2","M3","M4","U1","UT"};
    return posits[x];
}

char* stg2str (stg_t x)
{   if (x<PSBSTG_PR || x>PSBSTG_RI) x = 0;
    char *stages[] = {"__","PR","FL","TU","RI"};
    return stages[x];
}

char* sta2str (sta_t x)
{   if (x<PSBSTA_PLAYING || x>PSBSTA_SEMPTY) x = 0;
    char *states[] = 
    {   "",  //"unkn", 
	"*play",
	" fold",
	"*alln",
	" sout",
	" semp",
    };
    return states[x];
}

char* col2str (col_t x)
{   if (x<PSBCs || x>PSBCc) x = 0;
    char *colors[] = {" ","s","h","d","c"};
    return colors[x];
}

char* val2str (val_t x)
{   if (x<PSBV1 || x>PSBVA) x = 0;
    char *values[] = {" ","1","2","3","4","5","6","7","8","9","T","J","Q","K","A"};
    return values[x];
}

char* rnk2str (rnk_t x)
{   if (x<PSBRNK_HC || x>PSBRNK_RF) x = 0;
    char *rankes[]= {"__","HC","1P","2P","3K","ST","FL","FH","4K","SF","RF"};
    return rankes[x];
}

char* evl2str (rnk_t x)
{   if (x<PSBEVL_WO || x>PSBEVL_BE) x = 0;
    char *evales[]= {"__","WO","BA","PA","GO","BE"};
    return evales[x];
}

char* psh2str (psh_t x)
{   if (x<PSB_BUTFOLD || x>PSB_GOSITOUT) x = 0;
    char *pushes[]= 
    {   "",
	"button fold",
	"button call",
	"button check",
	"button raise",
	"button i'm back",
	"presel ftab",
	"presel sonh",
	"view ftab/sonh",
	"hide ftab/sonh",
	"page of info",
	"page of chat",
	"box of chat",
	"box of bet",
	"presel min",
	"presel 3bb",
	"presel pot",
	"presel max",
	"chat's box",
	"bet's box",
	"betting min",
	"betting 3bb",
	"betting pot",
	"betting max",
	"check or fold",
	"fold anyway",
	"sitting out"
    };
    return pushes[x];
}

static int printout0 (psb_t *tb, FILE *fp) 
{   if (!fp) {error(0,0,"printout0: null pointer"); return 0;}
    sit_t se = Seats(tb);
    int j;
        
    //system ("clear");   
    fprintf(fp,
	   "/-@ psbot0 @--------------------------------\\\n"
	   "| round[ %-8d ] rndtm[ %-8ld ] [ %d ] |\n"
	   "| yturn[ %d  ] plsin[ %d  ] posit[ %-2s ] nwrnd |\n"
           "| bfold[ %d  ] plsfo[ %d  ] stage[ %-2s ] [ %d ] |\n"
	   "| bcall[ %d  ] plsnt[ %d  ] sg.co[ %-2d ] sg.nw |\n"
	   "| check[ %d  ] plsyt[ %d  ]      [    ] [ %d ] |\n"
	   "| bback[ %d  ] plsso[ %d  ]      [    ] rsepr |\n"
	   "| sftab[ %d  ] plsai[ %d  ]      [    ] [ %d ] |\n" 
	   "| tinfo[ %d  ] empty[ %d  ]      [    ] cllpr |\n"
	   "| lastp[ %d  ]      [    ] pp_in[ %-2d ]       |\n",
	   tb->round, tb->rndtm, tb->nwrnd, 
	   tb->yturn, tb->plsin, pos2str(tb->posit), 
	   tb->bfold, tb->plsfo, stg2str(tb->stage), tb->sg.nw, 
	   tb->bcall, tb->plsnt, tb->sg.co,   
	   tb->check, tb->plsyt, tb->rsepr,
	   tb->bback, tb->plsso, tb->sftab, tb->plsai, tb->cllpr, 
	   tb->tinfo, tb->empty, tb->lastp, tb->pp_in);
    
    fprintf(fp,
	   "| tocll[ %11.2f%c] torse[ %11.2f%c] |\n"
           "| pot_t[ %11.2f%c] pot_s[ %11.2f%c] |\n"
	   "| cards[ %s%s %s%s %s%s %s%s %s%s %s%s %s%s ]             |\n",
         //"| sdrac[ %s%s %s%s %s%s %s%s %s%s %s%s %s%s ]             |\n",
#define bb ((tb->cf.sinbb)? 'b' : ' ')
	   tb->tocll, bb, tb->torse, bb,
	   tb->pot_t, bb, tb->pot_s, bb,
	   val2str(tb->cards[0].v), col2str(tb->cards[0].c),
	   val2str(tb->cards[1].v), col2str(tb->cards[1].c),
	   val2str(tb->cards[2].v), col2str(tb->cards[2].c),
	   val2str(tb->cards[3].v), col2str(tb->cards[3].c),
	   val2str(tb->cards[4].v), col2str(tb->cards[4].c),
	   val2str(tb->cards[5].v), col2str(tb->cards[5].c),
	   val2str(tb->cards[6].v), col2str(tb->cards[6].c));
         //  val2str(tb->sdrac[0].v), col2str(tb->sdrac[0].c),
	 //  val2str(tb->sdrac[1].v), col2str(tb->sdrac[1].c),
	 //  val2str(tb->sdrac[2].v), col2str(tb->sdrac[2].c),
	 //  val2str(tb->sdrac[3].v), col2str(tb->sdrac[3].c),
	 //  val2str(tb->sdrac[4].v), col2str(tb->sdrac[4].c),
	 //  val2str(tb->sdrac[5].v), col2str(tb->sdrac[5].c),
	 //  val2str(tb->sdrac[6].v), col2str(tb->sdrac[6].c));
         
    fprintf(fp,
	   "> settable vars >---------------------------<\n"
	   "| bl_bb[ %11.2f ] bl_sb[ %11.2f ] |\n"
	   "| pp_ra[ %-2s ] pp_po[ %-2s ] tmbnk[ %-2d ]       |\n",
	   tb->bl_bb, tb->bl_sb, 
	   pos2str(tb->pp_ra), pos2str(tb->pp_po), tb->tmbnk);

    fprintf(fp,
	   ">/se/rn/-/ide/-/sta/pos/sfl/--------/stack/-<\n");
    for (j=1; j<=se; j++)
           fprintf(fp,"|%2d[%3d %5s %5s %3s %3d %13.2f%c] |\n", 
	    j, tb->se[j].round, tb->se[j].ident, sta2str(tb->se[j].state), 
	    pos2str(sit2pos(tb,j)), tb->se[j].sawfl, tb->se[j].stack, bb);
            
    fprintf(fp,
	   "> cards ranking >---------------------------<\n"
           "| hrank[ %-2s ]  heval[ %-2s ]       [    ]     |\n"
	   "| hbest[ %s%s %s%s %s%s %s%s %s%s ]                   |\n" 
	   "| rdraw[ %-2s ]  edraw[ %-2s ]       [    ]     |\n",
           rnk2str(tb->hrank), rnk2str(tb->heval), 
           val2str(tb->hbest[0].v), col2str(tb->hbest[0].c),
           val2str(tb->hbest[1].v), col2str(tb->hbest[1].c),
           val2str(tb->hbest[2].v), col2str(tb->hbest[2].c),
           val2str(tb->hbest[3].v), col2str(tb->hbest[3].c),
           val2str(tb->hbest[4].v), col2str(tb->hbest[4].c),
	   rnk2str(tb->rdraw), rnk2str(tb->edraw));

    if (tb->tinfo)
    {   fprintf(fp,
	   "> tourney info >----------------------------<\n"
	   "| place[ %-5d] total[ %-5d] paidp[ %-5d] |\n"
	   "| stave[ %11.0f%c] stbig[ %11.0f%c] |\n"
	   "| blind[ %-10.0f %-10.0f %-10.0f ] |\n",
	   tb->ti.place, tb->ti.total, tb->ti.paidp, tb->ti.stave,bb, 
	   tb->ti.stbig,bb, tb->ti.bl_bb, tb->ti.bl_sb, tb->ti.antes); 
    }
    
    if (tb->cf.shwcf)
    {   fprintf(fp,
	   "> config stuffs >---------------------------<\n"
	   "| seats[ %d]  kepts[ %d] dealer[ %d] debug[ %d] |\n"
	   "| sinbb[ %d]  flpnt[ %d]       [  ]      [  ] |\n"
	   "| topos[ %2s %2s %2s %2s %2s %2s %2s %2s %2s %2s ]    |\n"
	   "| tosit[ %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d ]    |\n"
	   "| dpy[%10p] win_w[%4d ] captm[%4d ] |\n"
	   "| win[%10p] win_h[%4d ]      [     ] |\n"
	   "| gamet[ %-11s ]      [ %-11s ] |\n",
	   tb->cf.seats, tb->cf.kepts, tb->cf.dealer, tb->cf.debug,
	   tb->cf.sinbb, tb->cf.flpnt, 
	   pos2str(tb->cf.topos[1]), pos2str(tb->cf.topos[2]), 
	   pos2str(tb->cf.topos[3]), pos2str(tb->cf.topos[4]), 
	   pos2str(tb->cf.topos[5]), pos2str(tb->cf.topos[6]), 
	   pos2str(tb->cf.topos[7]), pos2str(tb->cf.topos[8]), 
	   pos2str(tb->cf.topos[9]), pos2str(tb->cf.topos[10]),
	   tb->cf.tosit[1], tb->cf.tosit[2], tb->cf.tosit[3], 
	   tb->cf.tosit[4], tb->cf.tosit[5], tb->cf.tosit[6],
	   tb->cf.tosit[7], tb->cf.tosit[8], tb->cf.tosit[9], 
	   tb->cf.tosit[10],
	   tb->cf.dpy, tb->cf.win_w, tb->cf.captm, (void*)Winid(tb), 
	   tb->cf.win_h, (Gamet(tb))?"":"tholdem", ""); 
    }
    
    fprintf(fp,"\\-------------------------------------------/\n");
    fflush(fp);
#undef bb    
    return 1;
}

int psb_show (psb_t *tb) 
{   if (!tb) error(EXIT_FAILURE,0,"psb_show: null pointer"); 
    return printout0(tb,stdout); 
}

void psb_info (psb_t *tb, const char *fmt, ...)
{   if (!tb || !fmt) error(EXIT_FAILURE,0,"psb_info: null pointers"); 
    va_list ap;
    va_start(ap, fmt);
    if (tb->cf.logout) 
    {   vfprintf(tb->cf.logout, fmt, ap); 
	fflush(tb->cf.logout); 
    }
    vfprintf(stdout, fmt, ap); fflush(stdout);
    va_end(ap);
    return;
}

pos_t str2pos (char *s)  
{   pos_t x=0; 
    for (x=10; x>=0; x--) if ( strstr(pos2str(x),s) ) return x;
    return x;
}

//library arguments parser----------------------------------------------------/
int psb_args (psb_t *tb, int argc, char **argv)
{   if (!tb || argc<0 || !argv) 
        error(EXIT_FAILURE,0,"psb_args: null pointers or args<0"); 
    int c, x=0;
    while ((c = getopt(argc, argv, "w:z:s:f:b:r:p:t:a:c:l:d:h")) != -1)
    switch (c)
    {	case 'w': tb->cf.win = parsewid(optarg); break;
        case 'z': tb->cf.wsize = atoi(optarg);   break;
        case 's': tb->cf.seats = atoi(optarg);   break;
	case 'f': tb->cf.flpnt = atoi(optarg);   break;
	case 'b': sscanf(optarg, "%f %f %f", &tb->bl_bb,&tb->bl_sb,&tb->antes);
		  break;
	case 't': x = atoi(optarg); tb->tmbnk = (x>=0 && x<=100)? x: 0; break;
	case 'p': tb->pp_po = str2pos(optarg);	 break;
	case 'r': tb->pp_ra = str2pos(optarg);   break;
	case 'a': tb->cf.sinbb = atoi(optarg);	 break;
	case 'c': tb->cf.shwcf = atoi(optarg);	 break;
	case 'l': tb->cf.logtb = atoi(optarg);	 break;
	case 'd': tb->cf.debug = atoi(optarg);   break;
	case 'h': 
  printf("\nSet initial values of psbot0 library.	 		 \n");
  printf("Usage: %s [option value]...				\n", argv[0]);
  printf("   -w ID   window id of table, 0:select with mouse (default)	   \n"
         "   -z N    width to resize table's window, 0:792(default)        \n"
         "             (-1):get actual size of window.		           \n"
         "   -s N    set number of seats in the table. [9(default),6,2]    \n"
         "   -f [01] 1 if size of amounts (stacks,pots,...) have a float   \n"
         "             point like in low stakes cash tables.		   \n"
         "   -b \"BB SB AN\" 						   \n"  
	 "           set fixed blinds, in tourney are setted automatically.\n"
         "   -t N    when turn to play, wait a percent N of banktime.      \n"
         "   -p PO   set a preflop range. [SB,BB,BU,CO,M1,M2,M3,M4,U1,UT]  \n"
         "   -r PO   set a preflop posit. 				   \n"
         "   -a [01] set size of amounts (stack,pots,...) in big blinds.   \n"
         "   -c [01] show some table's configs to output.                  \n"
         "   -l [01] save output and table's capts in a temporal unique    \n"
         "             directory named /tmp/psbot0-XXXXXX (default).       \n"
         "   -d N    show and capt debug info to /tmp/psbot0-debug         \n"
         "             0:none(default) 1:more 2:all.			   \n"
         "   -h      show this help and exiting ;).		       \n\n");
  exit(0);
	default: exit(0);
    }
    return 0;
}

//----------------------------------------------------------------------------/

