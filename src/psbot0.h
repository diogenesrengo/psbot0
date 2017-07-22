
#ifndef PSBOT0_H
#define PSBOT0_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h> //sleep()
#include <time.h>   
#include <assert.h>

//enumerations definitions----------------------------------------------------/
typedef enum    //[char name] [other comments]
{   PSBSITnu=-1,//u
    PSBSITxx=0, //0 unknown seat
    PSBSIT1=1,  //1 seat 1, my seat 
    PSBSIT2,    //2 seat 2, counter-clockwise direction 
    PSBSIT3,  
    PSBSIT4,
    PSBSIT5,
    PSBSIT6,
    PSBSIT7,
    PSBSIT8,
    PSBSIT9,
    PSBSIT10    //10 seat 10 
} sit_t;   
/* Fixed sitting names ----------------/---------------------------------------\
|               /(5)-(6)\              |               /--(4)--\               |
|        ...  (4)       (7)            |         ... (3)       (5)             |
|       SIT8 (3)         (8)           |              |         |              |
|       SIT9  (2)       (9) SIT2       |        SIT6 (2)       (6) SIT2        |
|               \--(1)--/              |               \--(1)--/               |
|               SIT1(ME)               |               SIT1(ME)                |
\--------------------------------------/--------------------------------------*/

typedef enum //relative position names         
{   PSBPOS_nu=-1,//u  
    PSBPOS_xx=0, //0 unknown posit
    PSBPOS_BB=1, //b big blind 
    PSBPOS_SB=2, //s small blind 
    PSBPOS_BU=3, //d button
    PSBPOS_CO=4, //c cutoff
    PSBPOS_M1=5, //1
    PSBPOS_M2=6, //2
    PSBPOS_M3=7, //3 middles 
    PSBPOS_M4=8, //4
    PSBPOS_U1=9, //5
    PSBPOS_UT=10,//u utg
    //
    PSBPOS_C0=4  //we accept wrong cutoff :)
} pos_t;
/*----------------------------------\
| relative position names according |
| (seats-emptys)                    |
.-----------------------------------.
| *2: BU BB                         |
|  3: BU BB SB                      |
|  4: BU UT BB SB                   |
|  5: BU U1 UT BB SB                |
| *6: BU CO U1 UT BB SB             |
|  7: BU CO M1 U1 UT BB SB          |
|  8: BU CO M1 M2 U1 UT BB SB       |
| *9: BU CO M1 M2 M3 U1 UT BB SB    |
| 10: BU CO M1 M2 M3 M4 U1 UT BB SB |
\----------------------------------*/

typedef enum //stages of round
{   PSBSTG_nu=-1,//u  
    PSBSTG_xx=0, //0 unknown stage
    PSBSTG_PR=1, //p preflop 
    PSBSTG_FL=2, //f flop 
    PSBSTG_TU=3, //t turn 
    PSBSTG_RI=4  //r river 
} stg_t;
			  
typedef enum //value of cards
{   PSBVnu=-1,//u 
    PSBVxx=0, //0 unknown value
    PSBV1=1,  //1
    PSBV2=2,  //2
    PSBV3=3,    
    PSBV4=4,    
    PSBV5=5,
    PSBV6=6,
    PSBV7=7,
    PSBV8=8,
    PSBV9=9,
    PSBVT=10, //T
    PSBVJ=11, //J
    PSBVQ=12, //Q
    PSBVK=13, //K
    PSBVA=14  //A
} val_t;

typedef enum //color of cards
{   PSBCnu=-1,//u 
    PSBCxx=0, //0 unknown color
    PSBCs=1,  //s spade, grey 
    PSBCh=2,  //h heart, red 
    PSBCd=3,  //d diamond, blue 
    PSBCc=4   //c clue, green 
} col_t;
		  
typedef enum //state of seat or player
{   PSBSTA_un=-1,  //u 
    PSBSTA_xx=0,   //0 unknown state
    PSBSTA_PLAYING,//i into round
    PSBSTA_FOLDOUT,//o fold hand
    PSBSTA_ALLIN,  //a all in
    PSBSTA_SITOUT, //s sit out
    PSBSTA_SEMPTY, //e sit empty
    //
    PSBSTA_NEWPLR, //n new player
} sta_t;

typedef enum //ranking of cards
{   PSBRNK_un=-1,//u  
    PSBRNK_xx=0, //0 unknown rank or eval
    //rank of hand 
    PSBRNK_HC=1, //c highcard 
    PSBRNK_1P,   //1 one pair 
    PSBRNK_2P,   //2 two pair 
    PSBRNK_3K,   //3 3 of kind 
    PSBRNK_ST,   //t straight 
    PSBRNK_FL,   //f flush 
    PSBRNK_FH,   //h full house 
    PSBRNK_4K,   //4 4 of kind 
    PSBRNK_SF,   //s straight flush
    PSBRNK_RF=10,//r royal flush 
    //hand and holds eval - OBSOLETE!!
    PSBEVL_WO=1, //w worst    
    PSBEVL_BA,   //d bad
    PSBEVL_PA,   //p pass  
    PSBEVL_GO,   //g good
    PSBEVL_BE=5, //b best  
} rnk_t;

typedef enum //clicking, writing and other on table
{   PSB_PUSHun=-1,//u 
    PSB_PUSHxx=0, //0  unknown 
    //buttons and presels and writings
    PSB_BUTFOLD=1,//bf click "Fold" button 
    PSB_BUTCALL,  //bc click "Call" button 
    PSB_BUTCHECK, //bk click "Check" button 
    PSB_BUTRAISE, //br click "Raise" button 
    PSB_BUTIBACK, //bi click "I'm back" button
    PSB_SELFTAB,  //sf select "Fold to any bet" 
    PSB_SELSONH,  //ss select "Sitout next hand"
    PSB_FTABVIEW, //fv slide - view ftab/sonh
    PSB_FTABHIDE, //fh slide - hide ftab/sonh
    PSB_PAGEINFO, //pi select info page
    PSB_PAGECHAT, //pc select chat page
    PSB_BOXCHAT,  //bc double click/select box of chat
    PSB_BOXBET,   //bb double click/select box of bet 
    PSB_SELBMIN,  //sm presel bet min
    PSB_SELB3BB,  //s3 presel bet 3bb
    PSB_SELBPOT,  //sp presel bet pot
    PSB_SELBMAX,  //sx presel bet max
    //writing  
    PSB_WRITECHAT,//wc write in box of chat, then pulse Enter
    PSB_WRITEBBET,//wb write in box of bet
    //shortcuts
    PSB_BETMIN,   //em presel then raise
    PSB_BET3BB,   //e3
    PSB_BETPOT,   //ep
    PSB_BETMAX,   //ex
    PSB_FOLDCHK,  //fc check else fold anyway
    PSB_FOLDANY,  //fa fold else ftab, fold anyway
    PSB_GOSITOUT, //so sitting out of table
} psh_t;

typedef enum
{   //my boolean type
    PSB_NOT=0,
    PSB_YES=1,
} noy_t;

//structures definitions------------------------------------------------------/
typedef struct 
{   //value and color of a card 
    val_t v;  
    col_t c;  
} card_t;

typedef struct 
{   //tourney info 
    float bl_bb, bl_sb, antes; //big and small blinds, ante
    int   place; //my position
    int   total; //remaining players 
    int   paidp; //paid positions
    float stave; //average stack
    float stbig; //biggest stack
} tinf_t;

typedef struct
{   //seat info 
    char   ident[5]; //seat identifier---------------------------------------/
    pos_t  posit;    //one fast, maybe unique, capture of player's name, if
    float  stack;    //ident changes it means that a new player it's sitting 
    sta_t  state;    //------------------------------------------------------/
    float  betpr; //preflop bet, blind
    int    round; //played rounds
    int    sfcnt; //saw flops counter
    int    sawfl; //% saw flop 
} seat_t;

typedef struct
{   //all stage info
    stg_t   stage; //stage of a round - same (psb_t).stage
    noy_t   nw;    //yes if it's a new stage
    int     co;    //in-same-stage counter, first time:0
} stgi_t;

typedef struct 
{   //all data and settings of one table 
/*@ begin user variables @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    noy_t   yturn; //your turn to play 
    noy_t   bfold; //fold button viewed 
    noy_t   bcall; //call button viewed
    noy_t   check; //check button viewed, you can check
    float   tocll; //amount in button call
    float   torse; //amount in button raise
    float   pot_t; //total pot
    float   pot_s; //stage pot 
    noy_t   bback; //i'm back button present
    noy_t   sftab; //ftab presel viewed - ftab: fold to any bet
    float   mystk; //my stack, same se[1].stack
    
    card_t  holds[2]; //holds cards sorted low to high
    card_t  comms[5]; //comms cards sorted low to high
    card_t  cards[7]; //all cards, holds first, no sorted
    card_t  sdrac[7]; //all cards sorted low to high
  
    seat_t  se[11]; //seats data - see seat_t type
    
    int     round; //total rounds, every hand played
    pos_t   posit; //my position in table - see pos_t enum
    noy_t   nwrnd; //new round, another holds cards viewed
    stg_t   stage; //stage of a round - same sg.stage
    stgi_t  sg;    //all stage info - see stgi_t struct
    	           //state of seats  
    sit_t   plsin; //players in, active    
    sit_t   plsfo; //players out, unknown state but no active
    sit_t   plsnt; //players in after me, they dont talk
    sit_t   plsyt; //players in before me, they talked	
    sit_t   plsai; //players allin
    sit_t   plsso; //players sitout
    sit_t   empty; //no players, sit empty label
        
    noy_t   rsepr; //only preflop - someone raise
    noy_t   cllpr; //only preflop - someone call
    noy_t   lastp; //last player to talk

    rnk_t   hrank; //rank of hand - see rnk_t enum
    card_t  hbest[5]; //best hand of all cards
    rnk_t   heval; //eval of best hand considering holds. poor eval, take care!
    rnk_t   rdraw; //draws of flush or straight, one card proyects, flush first
    rnk_t   edraw; //eval of best proyected hand (simulated)

    noy_t   tinfo; //tournament info viewed
    tinf_t  ti;    //tournament info
    noy_t   pp_in; //in range or position preflop - see psb_inpp()
    	
    time_t  rndtm; //elapsed average time between consecutive rounds, seconds

    //settable variables 
    int     tmbnk; //timebank - percent of bank to wait  
    pos_t   pp_ra; //preflop range // fixed blinds ----------------------------
    pos_t   pp_po; //preflop posit //in cash if bl_bb=0 we try to find them,
    float   bl_bb, bl_sb, antes;   //in tourney are automatic from info page
    
    struct 
    {   //table's configurations ---------------------------------------------/
        int     wsize; //set width (size) of table's window 
        	       //flpnt: if yes, size of amounts (stacks, pots, ...) 
	noy_t   flpnt; //have a float point like in low stakes cash tables
	noy_t   sinbb; //if 1, size of amounts in bb, (stacks, pots, ...)
        noy_t   shwcf; //if 1, show some config info of table to output
        noy_t   logtb; //if 1, save output and table capts in tmp directory
        int     debug; //show debug info, 0:none 1:more >2:all
/*@ ending user variables @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

        sit_t   seats;  //total seats, [9(default),6,2].     
        sit_t   kepts;  //busy seats, not sitempty. update every round
	sit_t   dealer; //dealer-button position 
	pos_t   topos[11]; //posit of seat (relative posit)
	sit_t   tosit[11]; //seat of posit (fixed seat) 
        enum { PSB_THOLDEM=0 } gamet; //game type, only texas holdem
	//X data of table's window	       
	Display *dpy;			       //width of table's window------/ 
	Window   win;			       //wsize= 0, default res: 792x546
	unsigned int win_w; //width of window  //wsize=-1, get actual win size	
	unsigned int win_h; //height of window //unexpected results if too small
	//  				       //-----------------------------/
	char *dbgdir; //directory for debugging, "/tmp/psbot0-debug"
	char *logdir; //log - directory, unique, "/tmp/psbot0-XXXXXX"
	FILE *logout; //log - file where to save output
    	//see psb0.h for types and macros
	void *xt; //xdo_t data - xdotool library
        void *gm; //gmtr_t data 
        void *ca; //capt_t data 
        void *ev; //eval_t data 
	//
	int  captm; //elapsed time to capture (psb_capt()), in milliseconds
	//--------------------------------------------------------------------/
    } cf;
} psb_t;

//functions declarations------------------------------------------------------/
psb_t* psb_psb0 (void);      //create a usable psb_t type (all zeros, alloc pts)
int    psb_free (psb_t *tb); //free memory of psb_t type
int    psb_args (psb_t *tb, int argc, char **argv); //parse args, init psb_t 
int    psb_init (psb_t *tb, Window id, int sits); //init table's stuffs-------/
//---------------------------------------/sleep, activate win table, wait turn, 
int    psb_loop (psb_t *tb, int delay); //capt and show. ret true when to play
int    psb_actw (psb_t *tb, int delay); //activate table's window and sleep---/
int    psb_turn (psb_t *tb); //it's you turn to play 
int    psb_capt (psb_t *tb); //capture all variables       
int    psb_show (psb_t *tb); //print data to stdout in nice format
//---------------------------------------/
int    psb_wait (psb_t *tb, int perc); //wait a banktime percent 
void   psb_info (psb_t *tb, const char *fmt, ...); //show info to log/out
Window parsewid (char  *sa); //get table's window id from string argument
Window byselwid (psb_t *tb); //get table's window id selecting with mouse 
int    psb_inpp (psb_t *tb, pos_t ra, pos_t po); //in preflop position
int    psb_prpo (psb_t *tb, pos_t ra, pos_t po); //fast, need psb_capt() first
int    psb_push (psb_t *tb, psh_t bu); //push buttons, selects
int    psb_wbox (psb_t *tb, psh_t bx, char *st, float am); //write in boxs
//
int    psb_prcs (psb_t *tb, char* sl); //preflop cards selections - see movs.c
int    psb_mrse (psb_t *tb, char *op, float mu); //raise
int    psb_mcll (psb_t *tb);			 //call
int    psb_mfld (psb_t *tb);			 //fold
//----------------------------------------------------------------------------/
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

//----------------------------------------------------------------------------/
#endif /* PSBOT0_H */

