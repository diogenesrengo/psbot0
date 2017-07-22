
#include <xdo.h>
#include "psb0.h"

//local function declarations-------------------------------------------------/
static inline int push_bfold (psb_t *tb);
static inline int push_raise (psb_t *tb);
static inline int push_bcall (psb_t *tb);
static inline int push_check (psb_t *tb);
static inline int push_bback (psb_t *tb);
static inline int push_sftab (psb_t *tb);
static inline int push_pinfo (psb_t *tb);
static inline int push_ssonh (psb_t *tb);
static inline int push_vftab (psb_t *tb);
static inline int push_hftab (psb_t *tb);
static inline int push_pbmin (psb_t *tb);
static inline int push_pb3bb (psb_t *tb);
static inline int push_pbpot (psb_t *tb);
static inline int push_pbmax (psb_t *tb);
static inline int push_pchat (psb_t *tb);
static inline int push_bocha (psb_t *tb);
static inline int push_bobet (psb_t *tb);
static inline int seeallins  (psb_t *tb);
static inline int write_cht  (psb_t *tb, char *st);
static inline int write_bet  (psb_t *tb, char *st);

//global function declarations------------------------------------------------/
int push_push (psb_t *tb, psh_t bu); //push buttons, selects
int push_wbox (psb_t *tb, psh_t bx, char *sp); //write in boxs: str or amount

//----------------------------------------------------------------------------/
#define DELAYC    250000 /*xdotool - delay between clicks, in microseconds*/
#define MSLEEP    sleep(1) 
#define MCLICK(n) xdo_move_mouse_relative_to_window(XTP(tb), Winid(tb), x, y); \
    	          xdo_click_window_multiple(XTP(tb), Winid(tb), 1, n, DELAYC)
#define DELAYW    250000 /*xdotool - delay to type chars, in microseconds*/
#define WSLEEP    sleep(1); 
#define WWRITE(s) xdo_enter_text_window(XTP(tb), Winid(tb), s, DELAYW) 
#define WENTER    xdo_send_keysequence_window(XTP(tb),Winid(tb),"Return",DELAYW)

#define CHTSLEEP  sleep(5) /*sleep after write in chat's box*/
#define BETSLEEP  sleep(0) 
//----------------------------------------------------------------------------/

#define pushIfyes(_id_,_cs_) \
    int x = GMP(tb)->_id_.x, y = GMP(tb)->_id_.y; \
    int ret = CAP(tb)->_id_; \
    if (ret) { MCLICK(_cs_); } \
    if (Debug(tb)) \
        fprintf(stderr,"push: "#_id_" ret:%d. x,y:%d,%d,\n",ret,x,y); \
    return ret 
#define pushIfnot(_id_,_cs_) \
    int x = GMP(tb)->_id_.x, y = GMP(tb)->_id_.y; \
    int ret = !CAP(tb)->_id_; \
    if (ret) { MCLICK(_cs_); } \
    if (Debug(tb)) \
        fprintf(stderr,"push: "#_id_" ret:%d. x,y:%d,%d,\n",ret,x,y); \
    return ret 

static inline int push_bfold (psb_t *tb) { pushIfyes(bfold,2); }

static inline int push_bcall (psb_t *tb)
{   int x = GMP(tb)->bcall.x, y = GMP(tb)->bcall.y;
    int ret = CAP(tb)->bcall || CAP(tb)->check;
    if (ret) { MCLICK(2); }
    if (Debug(tb))
	fprintf(stderr, "push: bcall/check ret:%d, x:%d, y:%d,\n", ret, x, y);
    return ret;
}

static inline int push_check (psb_t *tb)
{   int x = GMP(tb)->bcall.x, y = GMP(tb)->bcall.y;
    int ret = CAP(tb)->check;
    if (ret) { MCLICK(2); }
    if (Debug(tb)) fprintf(stderr,"push: check ret:%d, x:%d, y:%d,\n",ret,x,y);
    return ret;
}

static inline int push_raise (psb_t *tb) { pushIfyes(raise,2); }
static inline int push_bback (psb_t *tb) { pushIfyes(bback,2); }
static inline int push_pinfo (psb_t *tb) { pushIfnot(pinfo,1); }
static inline int push_sftab (psb_t *tb) { push_vftab(tb); pushIfyes(sftab,1); }

static inline int push_ssonh (psb_t *tb)
{   int x = GMP(tb)->ssonh.x, y = GMP(tb)->ssonh.y;
    push_vftab(tb); 
    //no needed capt function because is always visible	
    //if (!CAP(tb)->ssonh) { MSLEEP; MCLICK(1); } 
    MSLEEP; 
    MCLICK(1); 
    if (Debug(tb))
    //fprintf(stderr,"push: ssonh ret:%d, x:%d, y:%d,\n",CAP(tb)->ssonh,x,y);
      fprintf(stderr,"push: ssonh ret:%d, x:%d, y:%d,\n", 1, x, y);
    return 1;
}

static inline int push_vftab (psb_t *tb)
{   int x = GMP(tb)->vftab.x, y = GMP(tb)->vftab.y;
    int ret = !CAP(tb)->hftab;
    if (ret) { MCLICK(1); }
    if (Debug(tb)) fprintf(stderr,"push: vftab ret:%d, x:%d, y:%d,\n",ret,x,y);
    return ret;
}

static inline int push_hftab (psb_t *tb) { pushIfyes(hftab,1); }

#define pushIfply(_id_,_cs_) \
    int x = GMP(tb)->_id_.x, y = GMP(tb)->_id_.y; \
    int ret = CAP(tb)->yturn; \
    if (ret) { MCLICK(_cs_); } /* visible only when is turn to play */ \
    if (Debug(tb)) \
        fprintf(stderr,"push: "#_id_" ret:%d, x:%d, y:%d,\n",ret,x,y); \
    return ret 

static inline int push_pbmin (psb_t *tb) { pushIfply(pbmin,1); }
static inline int push_pb3bb (psb_t *tb) { pushIfply(pb3bb,1); }
static inline int push_pbpot (psb_t *tb) { pushIfply(pbpot,1); }
static inline int push_pbmax (psb_t *tb) { pushIfply(pbmax,1); }

static inline int push_pchat (psb_t *tb) { pushIfnot(pchat,1); }

static inline int push_bocha (psb_t *tb)
{   int x = GMP(tb)->bocha.x, y = GMP(tb)->bocha.y;
    if (!CAP(tb)->pchat) push_pchat(tb);
    MSLEEP;
    MCLICK(2);
    if (Debug(tb)) fprintf(stderr,"push: bocha ret:%d, x:%d, y:%d,\n",1,x,y);
    return 1;
}

static inline int push_bobet (psb_t *tb) { pushIfyes(bobet,2); }

static inline int seeallins (psb_t *tb)
{   int j, ret=0;
    char *sp=0;
    if (tb->plsai) ret=1; else //fast but maybe dont setted, need psb_capt()
    { capt_state(tb); //stack ident empty
      for (j=1; j<=Seats(tb); j++) 
      {   sp = CAP(tb)->stack[j]; 
          if (!CAP(tb)->empty && !strstr(sp,"A")) { ret=1; break; }
      }
    }
    if (Debug(tb)) fprintf(stderr, "push: seeallins ret:%d,\n", ret);
    return ret;	
}

static inline int write_cht (psb_t *tb, char *st)
{   int ret = (push_bocha(tb) && !seeallins(tb));
    if (ret)
    {   WSLEEP;
	WWRITE(st); 
	WENTER;
    }
    if (Debug(tb)) fprintf(stderr,"push: write chat ret:%d, st:%s,\n",ret,st);
    return ret;
}

static inline int write_bet (psb_t *tb, char *st)
{   int ret = push_bobet(tb);
    if (ret)
    {   WWRITE(st);
        //WENTER; //dont know what happen here
	//push_raise(tb);
    }
    if (Debug(tb)) fprintf(stderr,"push: write bet ret:%d, st:%s,\n",ret,st);
    return ret;
}

//----------------------------------------------------------------------------/
int psb_push (psb_t *tb, psh_t bu)
{   if (!tb) {error(0,0,"psb_push: null pointer"); return 0;}
    int ret = 0;

    gmtr_calc0(tb);
    capt_captp(tb);

    switch (bu)
    { case PSB_BUTFOLD:  ret = push_bfold(tb); break;
      case PSB_BUTCALL:  
      case PSB_BUTCHECK: ret = push_bcall(tb); break;
      case PSB_BUTRAISE: ret = push_raise(tb); break;
      case PSB_BUTIBACK: ret = push_bback(tb); break;
      case PSB_SELBMIN:  ret = push_pbmin(tb); break;
      case PSB_SELB3BB:  ret = push_pb3bb(tb); break;
      case PSB_SELBPOT:  ret = push_pbpot(tb); break;
      case PSB_SELBMAX:  ret = push_pbmax(tb); break;
      case PSB_SELFTAB:  ret = push_sftab(tb); break;
      case PSB_SELSONH:  ret = push_ssonh(tb); break;
      case PSB_FTABVIEW: ret = push_vftab(tb); break;
      case PSB_FTABHIDE: ret = push_hftab(tb); break;
      case PSB_PAGEINFO: ret = push_pinfo(tb); break;
      case PSB_PAGECHAT: ret = push_pchat(tb); break;
      case PSB_BOXCHAT:  ret = push_bocha(tb); break;
      case PSB_BOXBET:   ret = push_bobet(tb); break;
      //shortcuts
      case PSB_BETMIN:   ret = (push_pbmin(tb) && push_raise(tb)); break;
      case PSB_BET3BB:   ret = (push_pb3bb(tb) && push_raise(tb)); break;
      case PSB_BETPOT:   ret = (push_pbpot(tb) && push_raise(tb)); break;
      case PSB_BETMAX:   ret = (push_pbmax(tb) && push_raise(tb)); break;
      case PSB_FOLDCHK:  ret = (push_check(tb) ||
                                push_bfold(tb) || push_sftab(tb)); break;
      case PSB_FOLDANY:  ret = (push_bfold(tb) || push_sftab(tb)); break;
      case PSB_GOSITOUT: ret = (push_sftab(tb) && push_ssonh(tb)); break;
      default: break;
    }    
    
    Info(tb,"# %spushing %s\n", (ret)?"":"not ", psh2str(bu));
    return ret;
}

int psb_wbox (psb_t *tb, psh_t bx, char *st, float am)
{   if (!tb) {error(0,0,"psb_wbox: null pointer"); return 0;}
    int ret=0;    
    char *sp=0, s0[32]={0};
    int stb = !(!st && (am<PSBZERO)); //something to bet 

    if (!st) { sprintf(s0, "%.2f", am); sp=s0; } else sp=st;
    gmtr_calc0(tb);
    capt_captw(tb);

    if (st || stb) 
      switch (bx)
      { case PSB_WRITECHAT: ret = write_cht(tb,sp); CHTSLEEP; break;
        case PSB_WRITEBBET: ret = write_bet(tb,sp); BETSLEEP; break;
        default: break;
      }
    
    Info(tb,"# %swriting in %s str:%s,\n", (ret)?"":"not ", psh2str(bx),sp);
    return (stb)? 1 : ret;
}

//----------------------------------------------------------------------------/

