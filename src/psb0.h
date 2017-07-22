
#ifndef PSB_PSB0_H
#define PSB_PSB0_H 1

#include <errno.h> 
#include <error.h>
#include <stdarg.h>
#include "psbot0.h"
#include "util.h"

/* structure definitions. ****************************************************/
typedef struct 
{   //defined in capt.c, captured table's data  
    float tocll; 
    float torse; 
    float pot_t; 
    float pot_s; 
    int vcard[8];
    int ccard[7];
    noy_t bfold;
    noy_t bcall;
    noy_t check;
    noy_t yturn;
    noy_t raise;
    noy_t bback;
    noy_t hftab;
    noy_t sftab;
    noy_t pchat;
    noy_t bobet;
	
    noy_t pinfo; //page info selected
    noy_t tinfo; //tournament info viewed
    int tplace;
    int ttotal;
    int tpaidp;
    int tblind[3]; //blinds in info
    int tstave;
    int tstbig;
    
    int tmbnk;
    int dealer;
    char ident[11][24];
    char stack[11][48];
    int  empty[11];
    float betpr[11]; //blinds in table, preflop bets
} capt_t;

typedef struct 
{   //geometry info
    char *id; //name of variable in gmtr_t, used in macros and debug mode
    struct { float x, y, w, h; } fg; //fractional geometry, for all window's size
    int x; //x offset
    int y; //y offset
    int w; //width
    int h; //height
} gmtx_t;

typedef struct 
{   //defined in gmtr.c, geometries of table's capts/pushs 
    gmtx_t tocll; 
    gmtx_t torse; 
    gmtx_t pot_t; 
    gmtx_t pot_s; 
    gmtx_t vcard[2];
    gmtx_t ccard[7];
    gmtx_t tplace;
    gmtx_t tpaidp;
    gmtx_t tblind; 
    gmtx_t tstave;
    gmtx_t bfold;
    gmtx_t bcall;
    gmtx_t check;
    gmtx_t raise;
    gmtx_t bback;
    gmtx_t sftab;
    gmtx_t pinfo; 
    gmtx_t tinfo; 
    gmtx_t pchat;
    //this is for push.c /
    gmtx_t ssonh;
    gmtx_t vftab;
    gmtx_t hftab;
    gmtx_t bocha; 
    gmtx_t bobet; 
    gmtx_t pbmin; 
    gmtx_t pb3bb; 
    gmtx_t pbpot; 
    gmtx_t pbmax;
    gmtx_t sinfo;
    //-------------------/
    gmtx_t tmbnk[10];
    gmtx_t dealer[11];
    gmtx_t stack[11];
    gmtx_t empty[11];
    gmtx_t betpr[11]; 
    gmtx_t ident[11];
} gmtr_t;

typedef struct 
{   //defined in eval.c, ranking of cards 
    rnk_t  hrank;   //rank of best hand, general 
    card_t bhand[5];//best hand ----------------------------------------------/
    int    heval;   //a poor evaluation of best hand considering holds, r:0-9  
		    //--------------------------------------------------------/
    int    fldrw;   //flush draw, 1:one card proyect, 2:two cards
    int    flevl;   //flush draw, same heval for flhnd
    card_t flhnd[5];//flush draw, best proyected fake hand 
    int    stdrw;   //
    int    stevl;   //straight draw, same as flush draw
    card_t sthnd[5];//

} eval_t;

/* macro definitions *********************************************************/
#define PSBZERO 0.001

#define printouts(p,o,r, ... ) do \
{   if ((p)->cf.logout) \
    {   fprintf((p)->cf.logout, ##__VA_ARGS__); fflush((p)->cf.logout); \
    } \
    fprintf(stdout, ##__VA_ARGS__); fflush(stdout); \
    switch (o) \
    {   case 'e': exit  (r); \
	case 'r': return(r); \
	default : ; \
    } \
} while (0) 
#define Info(p,   ... ) printouts(p,   0, 0, ##__VA_ARGS__)
#define Quit(p,r, ... ) printouts(p, 'e', r, ##__VA_ARGS__)

#define Debug(p) (p)->cf.debug
#define Flpnt(p) (p)->cf.flpnt //there's float point in sizes
#define Seats(p) (p)->cf.seats
#define Gamet(p) (p)->cf.gamet //type of game, only texas holdem 
#define Winid(p) (p)->cf.win   //window's identifier
#define Win_w(p) (p)->cf.win_w //window's width
#define Win_h(p) (p)->cf.win_h //window's height
#define GMP(p) ((gmtr_t*)(p)->cf.gm) //
#define CAP(p) ((capt_t*)(p)->cf.ca) // pointers to internal data of psbot0
#define EVP(p) ((eval_t*)(p)->cf.ev) //
#define XTP(p) ((xdo_t *)(p)->cf.xt) //

/* function declarations *****************************************************/
int capt_capt0 (psb_t *tb); //psb0: psb_capt()
int capt_state (psb_t *tb); //psb0: getstates()
int capt_captt (psb_t *tb); //psb0: psb_wait()
int capt_stage (psb_t *tb); //psb0: findstage()
int capt_captp (psb_t *tb); //push: psb_push()
int capt_captw (psb_t *tb); //push: psb_wbox() 
int capt_captu (psb_t *tb); //psb0: psb_turn()
int capt_captb (psb_t *tb); //psb0: getstates() getblinds()
int capt_save0 (psb_t *tb, gmtx_t *gm, float resize, int togrey);
int capt_gmtr0 (psb_t *tb); //debug: draw geometries in gmtr0.png
int eval_eval0 (psb_t *tb);
int getwinsize (psb_t *tb); 
int setwinsize (psb_t *tb, unsigned int w, unsigned int h);
int gmtr_calc0 (psb_t *tb); //reset geometries if window size changes
int sort_cards (card_t *cs, int len, int cmp); //sort cards, cmp:0 low to high

/*****************************************************************************/
#endif /* PSB_PSB0_H */

