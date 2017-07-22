
#include <X11/Xutil.h>
#include <wand/MagickWand.h>
#include "mocr.h"
#include "psb0.h"

/* local function declarations. **********************************************/
static imag_t* 
	crop_text (psb_t *tb, gmtx_t *gm, float resize, int thresh, int negate); 
static unsigned char 
	crop_grey (psb_t *tb, gmtx_t *gm); 
static inline void capt_tocll (psb_t *tb);
static inline void capt_torse (psb_t *tb);
static inline void capt_pot_t (psb_t *tb);
static inline void capt_pot_s (psb_t *tb);
static inline void capt_vcard (psb_t *tb);
static inline void capt_place (psb_t *tb);
static inline void capt_paidp (psb_t *tb);
static inline void capt_blind (psb_t *tb);
static inline void capt_stave (psb_t *tb);
static inline void capt_bfold (psb_t *tb);
static inline void capt_bcall (psb_t *tb);
static inline void capt_raise (psb_t *tb);
static inline void capt_bback (psb_t *tb);
static inline void capt_hftab (psb_t *tb);
static inline void capt_sftab (psb_t *tb);
static inline void capt_pinfo (psb_t *tb);
static inline void capt_pchat (psb_t *tb);
static inline void capt_bobet (psb_t *tb);
//---------------------------------------/
static inline void capt_ccard (psb_t *tb);
static inline void capt_tmbnk (psb_t *tb);
static inline void capt_dealer(psb_t *tb);
static inline void capt_stack (psb_t *tb);
static inline void capt_betpr (psb_t *tb);
static inline void capt_empty (psb_t *tb);
static inline void capt_ident (psb_t *tb);
//---------------------------------------/
/* global function declarations. *********************************************/
int capt_capt0 (psb_t *tb); //psb0: psb_capt()
int capt_state (psb_t *tb); //psb0: getstates()
int capt_captt (psb_t *tb); //psb0: psb_wait()
int capt_stage (psb_t *tb); //psb0: findstage()
int capt_captp (psb_t *tb); //push: psb_push()
int capt_captw (psb_t *tb); //push: psb_wbox() 
int capt_captu (psb_t *tb); //psb0: psb_turn()
int capt_captb (psb_t *tb); //psb0: getstates() getblinds()

int capt_save0 (psb_t *tb, gmtx_t *gm, float resize, int togrey); 
int capt_gmtr0 (psb_t *tb);  //debug: draw geometries in gmtr0.png

/*****************************************************************************/
static imag_t* crop_text (psb_t *tb, gmtx_t *gm, float res, int thr, int neg) 
{   if (!gm || !tb) {error(0,0,"crop_text: null pointers"); return 0;}
    char sn[48];
    XImage *xp=0;
    MagickWand *mp=0;
    imag_t *im=0;
    int x=gm->x, y=gm->y, w=gm->w, h=gm->h;

    if (Debug(tb)>1)
    {   fprintf(stderr, "capt: imag: win_w=%d win_h=%d display=%p window=%lx\n", 
	    Win_w(tb), Win_h(tb), tb->cf.dpy, Winid(tb));
        fprintf(stderr, "capt: imag: f=%f,%f,%f,%f, g=%d,%d,%d,%d,\n", 
	    gm->fg.x, gm->fg.y, gm->fg.w, gm->fg.h, x, y, w, h);
    }
    xp = XGetImage(tb->cf.dpy, Winid(tb), x,y,w,h, AllPlanes, ZPixmap);
    if (!xp) {error(0,0,"crop_text: fail to get image from X"); return 0;}
    MagickWandGenesis();
    mp = NewMagickWand();
    if (!mp) 
    {   error(0,0,"crop_text: failed to create new image (Magick)");
	goto CROPQUIT;
    }
    MagickConstituteImage(mp, w, h, "BGRP", CharPixel, xp->data);
//-----------------------------------------------------------------------------
#define CROPDBG0(x) if (Debug(tb) && gm->id) do \
{   sprintf(sn, "%s/%s"#x".png", tb->cf.dbgdir, gm->id); \
    MagickWriteImage(mp, sn); \
} while (0)
//-----------------------------------------------------------------------------
    //CROPDBG0(_0);
    //transforms resize,thresh,negate
    if (res) 
    {	w = w * res; 
    	h = h * res;
	MagickResizeImage(mp, w, h, TriangleFilter, 1); //CROPDBG0(_1-res);
    }
    MagickTransformImageColorspace(mp, GRAYColorspace); CROPDBG0(_2-gry);    
    if (thr) { MagickThresholdImage(mp, thr);           CROPDBG0(_3-thr); }
    if (neg) { MagickNegateImage(mp, MagickFalse);      CROPDBG0(_4-neg); }
    //
    im = malloc( sizeof(unsigned char) );
    if (!im) 
    {   error(0,0,"crop_text: failed to assign memory per im");
	goto CROPQUIT;
    }
    im->x = w;
    im->y = h;
    im->p = malloc( w * h * sizeof(unsigned char) );
    if (!im->p) 
    {	error(0,0,"crop_text: failed to assign memory per im->p.");
	goto CROPQUIT;
    }
    MagickExportImagePixels(mp, 0, 0, w, h, "I", CharPixel, im->p);
CROPQUIT:    
    XDestroyImage(xp);
    DestroyMagickWand(mp);
    MagickWandTerminus();
    return im; 
} 

#define crop_free(im) if(im){ if(im->p)free(im->p); free(im); }

static unsigned char crop_grey (psb_t *tb, gmtx_t *gm) 
{   if (!gm || !tb) {error(0,0,"crop_grey: null pointers"); return 0;}
    int i, j;
    unsigned char rgb[3]={0,0,0};
    XImage *xp=0;
    int x=gm->x, y=gm->y;

    if (Debug(tb)>1)
    {   fprintf(stderr, "capt: imag: win_w=%d win_h=%d display=%p window=%lx\n", 
	    Win_w(tb), Win_h(tb), tb->cf.dpy, Winid(tb));
        fprintf(stderr, "capt: imag: f=%f,%f,%f,%f, g=%d,%d,%d,%d,\n", 
	    gm->fg.x, gm->fg.y, gm->fg.w, gm->fg.h, x, y, gm->w, gm->h);
    }
    xp = XGetImage(tb->cf.dpy, Winid(tb), x,y,1,1, AllPlanes, ZPixmap);
    if (!xp) {error(0,0,"crop_grey: fail to get image from X"); return 0;}
    for (i=0,j=2; i<3; i++,j--) 
    {	rgb[j]= xp->data[i];
	if      (rgb[j] == -1) rgb[j]=255; 
	else if (rgb[j]  <  0) rgb[j]=rgb[j]*(-1)+127;
    }
    XDestroyImage(xp);
/*
    if (Debug(tb) && gm->id)
    {	gmtx_t g0=*gm;
	g0.w = 20;
	g0.h = 20;
	if (g0.x<(g0.w/2)) g0.x = 0; else g0.x -= (g0.w/2); 
	if (g0.y<(g0.h/2)) g0.y = 0; else g0.y -= (g0.h/2); 
	int ww=Win_w(tb), wh=Win_h(tb); 
	if (g0.x>=(ww-g0.w)) g0.x -= (ww-g0.w/2); 
	if (g0.y>=(wh-g0.h)) g0.y -= (wh-g0.h/2); 
	imag_t *im = crop_text(tb,&g0,0,0,0); 
	crop_free(im);
    }
*/
    //mix rgb colors 	
    return (unsigned char)(0.21*rgb[0] + 0.71*rgb[1] + 0.08*rgb[2]);
}

//tranformations in crops (MagickWand) resize,thresh,negate
#define TRANtocll  0,32000,1 //tocll torse  
#define TRANholds  0,48000,1 //holds comms 
#define TRANpot_t  2,32000,1 //pot_t 
#define TRANpot_s  3,32000,1 //pot_s 
#define TRANmystk  2,32000,1 //my stack  
#define TRANstack  2,33000,1 //stack  
#define TRANident  0,32000,1 //ident 
#define TRANbetpr  3,32000,1 //betpr
#define TRANtinfo  0,0,0     //info: place paidp blind stave tinfo 
#define TRANbfold  0,32000,1
#define TRANbcall  0,32000,1
#define TRANcheck  0,32000,1
#define TRANbback  0,32000,1
//recognition options of gocr - see mocr.h
#define GOCRholds  "0-9AKQJ",  "4",0,0,0,85 //holds comms 
#define GOCRtocllt "0-9" ,      "",0,0,0, 0 //tocll torse pot_t pot_s 
#define GOCRstackt "0-9An",     "",0,0,0, 0 //stacks
#define GOCRtocllc "0-9.," ,    "",0,0,0, 0 //cash: tocll torse pot_t pot_s  
#define GOCRstackc "0-9An,.",   "",0,0,0, 0 //cash: stack
#define GOCRplace  "0-9",      " ",0,0,0,97 //tour: place, 5 per s 
#define GOCRtourni "0-9",      " ",0,0,0, 0 //tour: paidp blind stave 
#define GOCRident  "0-9a-zA-Z", "",0,0,0, 0 //ident 
#define GOCRbetprt "0-9",       "",0,0,0, 0 //betpr 
#define GOCRbetprc "0-9.,",     "",0,0,0, 0 //cash: betpr
#define GOCRbfold  "Fl",        "",0,0,0,90
#define GOCRbcall  "Cl",        "",0,0,0,90
#define GOCRcheck  "Ch",        "",0,0,0,90
#define GOCRbback  "Bc",        "",0,0,0,90
#define GOCRtinfo  "Bl",        "",0,0,0,90

#define CAPTa(tb,id,go,tr) \
    char st[40]={0}; \
    gopt_t opt={GOCR##go##t}, opc={GOCR##go##c}, *op=0; \
    imag_t *im=0; \
    int nl=0; \
    \
    if (!Flpnt(tb)) op=&opt; else op=&opc; \
    im = crop_text(tb, &(GMP(tb)->id), TRAN##tr); \
    nl = gocr_call(op, im, st, 40); \
    remchars(st, ' '); \
    CAP(tb)->id = (st[0])? atof(st) : 0; \
    if (Debug(tb)) \
    {   fprintf(stderr, "capt: "#id":%.2f, st:%s, nl:%d, tp:%s,\n", \
	  CAP(tb)->id, st, nl, (Flpnt(tb))?"float":"integ"); \
    } \
    crop_free(im); \
    return

static inline void capt_tocll (psb_t *tb) { CAPTa(tb,tocll,tocll,tocll); }
static inline void capt_torse (psb_t *tb) { CAPTa(tb,torse,tocll,tocll); }
static inline void capt_pot_t (psb_t *tb) { CAPTa(tb,pot_t,tocll,pot_t); }
static inline void capt_pot_s (psb_t *tb) { CAPTa(tb,pot_s,tocll,pot_s); }

static inline void capt_vcard (psb_t *tb)
{   char st[48]={0}, cs[8]={0};
    gopt_t op={GOCRholds};
    imag_t *im=0;
    int j, nl=0;
    val_t v=0;
    
    //in headups, changes a little the geometry of holds cards
    //if (Seats(tb) == 2) fh.y -= (17*fx.h); 
    for (j=0; j<7; j++) cs[j] = ' ';
    im = crop_text(tb, &(GMP(tb)->vcard[0]), TRANholds);
    nl = gocr_call(&op, im, st, 16);
    if (Debug(tb)) fprintf(stderr, "capt: holds: st:%s, nl:%d,\n", st, nl);
    remchars(st, ' ');
    remchars(st, '0');
    for (j=0; j<2&&st[j]; j++) cs[j] = st[j];
    crop_free(im);
    
    st[0] = 0;
    im = crop_text(tb, &(GMP(tb)->vcard[1]), TRANholds);
    nl = gocr_call(&op, im, st, 48);
    if (Debug(tb)) fprintf(stderr, "capt: comms: st:%s, nl:%d,\n", st, nl); 
    remchars(st, ' ');
    remchars(st, '0');
    for (j=0; j<5&&st[j]; j++) cs[j+2] = st[j];
    crop_free(im);
    
    cs[7] = 0; 
    for (j=0; j<7; j++)
    {	switch (cs[j])
        {   case ' ': v = 0;     break;
  	    case '1': v = PSBVT; break; 
	    case 'J': v = PSBVJ; break;
	    case 'Q': v = PSBVQ; break;
	    case 'K': v = PSBVK; break; 
	    case 'A': v = PSBVA; break;
	    default : v = cs[j]-'0';
        }
	CAP(tb)->vcard[j] = v;
    }
    if (Debug(tb)) for (j=0;j<7;j++) 
	fprintf(stderr, "capt: vcard[%d]:%d(%c),\n", j,CAP(tb)->vcard[j],cs[j]); 
    
    return;
}

static inline void capt_place (psb_t *tb)
{   char st[40]={0};
    char *tk[10];
    int nt=0, nl=0;
    gopt_t op={GOCRplace};
    imag_t *im=0;
    
    im = crop_text(tb, &(GMP(tb)->tplace), TRANtinfo);
    nl = gocr_call(&op, im, st, 40);
    if (Debug(tb)) fprintf(stderr, "capt: tplace: st:%s, nl:%d,\n", st, nl);
    nt = splittok(st, tk, " ");
    CAP(tb)->tplace = (nt  )? atoi(tk[0]): 0; 
    CAP(tb)->ttotal = (nt>2)? atoi(tk[2]): 0;
    if (Debug(tb)) 
    {   fprintf(stderr, "capt: tplace:%d,\n", CAP(tb)->tplace);
	fprintf(stderr, "capt: ttotal:%d,\n", CAP(tb)->ttotal);
    }
    crop_free(im);
    return;
}

static inline void capt_paidp (psb_t *tb)
{   char st[40]={0};
    char *tk[16];
    int nt=0, nl=0;
    gopt_t op={GOCRtourni};
    imag_t *im=0;
    
    im = crop_text(tb, &(GMP(tb)->tpaidp), TRANtinfo);
    nl = gocr_call(&op, im, st, 40);
    if (Debug(tb)) fprintf(stderr, "capt: tpaidp: st:%s, nl:%d,\n", st, nl);
    nt = splittok(st, tk, " ");
    CAP(tb)->tpaidp = (nt)? atoi(tk[0]): 0; 
    if (Debug(tb)) fprintf(stderr, "capt: tpaidp:%d,\n", CAP(tb)->tpaidp);
    crop_free(im);
    return;
}

static inline void capt_blind (psb_t *tb)
{   char st[40]={0};
    char *tk[16];
    int nt=0, nl=0;
    gopt_t op={GOCRtourni};
    imag_t *im=0;

    im = crop_text(tb, &(GMP(tb)->tblind), TRANtinfo);
    nl = gocr_call(&op, im, st, 40);
    if (Debug(tb)) fprintf(stderr, "capt: tblind: st:%s, nl:%d,\n", st, nl);
    nt = splittok(st, tk, " ");
    CAP(tb)->tblind[0] = (nt>1)? atoi(tk[1]): 0;
    CAP(tb)->tblind[1] = (nt  )? atoi(tk[0]): 0;
    CAP(tb)->tblind[2] = (nt>2)? atoi(tk[2]): 0;
    if (Debug(tb))
    {	fprintf(stderr, "capt: tblind:%d,%d,%d,\n", CAP(tb)->tblind[0], 
	  CAP(tb)->tblind[1], CAP(tb)->tblind[2]);
    }
    crop_free(im);
    return;
}

static inline void capt_stave (psb_t *tb)
{   char st[40]={0};
    char *tk[10];
    int nt=0, nl=0;
    gopt_t op={GOCRtourni};
    imag_t *im=0;

    im = crop_text(tb, &(GMP(tb)->tstave), TRANtinfo);
    nl = gocr_call(&op, im, st, 40);
    if (Debug(tb)) fprintf(stderr, "capt: tstave: st:%s, nl:%d,\n", st, nl);
    nt = splittok(st, tk, " ");
    CAP(tb)->tstave = (nt>4)? atoi(tk[4]): 0; 
    CAP(tb)->tstbig = (nt>2)? atoi(tk[2]): 0;
    if (Debug(tb)) 
    {   fprintf(stderr, "capt: tstave:%d,\n", CAP(tb)->tstave);
	fprintf(stderr, "capt: tstbig:%d,\n", CAP(tb)->tstbig);
    }
    crop_free(im);
    return;
}

/*****************************************************************************/
#define yesIfGrey(_id_,grey) do \
{   unsigned char gr=0; \
    gr = crop_grey(tb, &(GMP(tb)->_id_)); \
    CAP(tb)->_id_ |= (gr /*comp*/ grey); \
    if (Debug(tb)) \
      fprintf(stderr, "capt: "#_id_":%d, gr:%d,\n", CAP(tb)->_id_, gr); \
} while(0) 
#define yesIfText(_id_,text) do \
{   char st[10]={0}; \
    gopt_t op={GOCR##_id_}; \
    imag_t *im = crop_text(tb, &(GMP(tb)->_id_), TRAN##_id_); \
    gocr_call(&op, im, st, 10); \
    CAP(tb)->_id_ |= ((st[0])? !strcmp(st,#text) : 0); \
    crop_free(im); \
    if (Debug(tb)) \
      fprintf(stderr, "capt: "#_id_":%d, st:%s,\n", CAP(tb)->_id_, st); \
} while(0)

static inline void capt_bfold (psb_t *tb) 
{   CAP(tb)->bfold = 0;
    yesIfGrey (bfold, >20);
    //yesIfText (bfold, Fl); 
    return;
}

static inline void capt_bcall (psb_t *tb) 
{   CAP(tb)->bcall = 0;
    CAP(tb)->check = 0;
    yesIfGrey (bcall, >20); 
    if (CAP(tb)->bcall) 
    {   yesIfText (check, Ch);
	CAP(tb)->bcall = !CAP(tb)->check;
    }
    return;
}
    
static inline void 
capt_raise (psb_t *tb) { CAP(tb)->raise=0; yesIfGrey(raise, >20); return; }

static inline void 
capt_bback (psb_t *tb) { CAP(tb)->bback=0; yesIfText(bback, Bc); return; }

static inline void 
capt_sftab (psb_t *tb) { CAP(tb)->sftab=0; yesIfGrey(sftab, >100); return; }

static inline void 
capt_hftab (psb_t *tb) { CAP(tb)->hftab=0; yesIfGrey(hftab, >30); return; }

static inline void capt_pinfo (psb_t *tb)
{   CAP(tb)->pinfo = 0; 
    CAP(tb)->tinfo = 0; 
    yesIfGrey (pinfo, <100);
    if (CAP(tb)->pinfo) yesIfText (tinfo, Bl);
    return;
}

static inline void 
capt_pchat (psb_t *tb) { CAP(tb)->pchat=0; yesIfGrey(pchat, <100); return; }

static inline void 
capt_bobet (psb_t *tb) { CAP(tb)->bobet=0; yesIfGrey(bobet, >30); return; }

/*****************************************************************************/
#define GR0 (gr<40)  //black(none)
#define GR1 (gr<84)  //grey(spade)
#define GR2 (gr<95)  //red(heart)
#define GR3 (gr<130) //blue(diamond) else green(clue)  
#define color_of_gr GR0?PSBCxx:( GR1?PSBCs:( GR2?PSBCh:( GR3?PSBCd:PSBCc )))

static inline void capt_ccard (psb_t *tb)
{   int j;
    unsigned char gr=0;
    
    for (j=0; j<7; j++)
    { gr = crop_grey(tb, &(GMP(tb)->ccard[j]));
      CAP(tb)->ccard[j] = color_of_gr;
      if (Debug(tb)) 
        fprintf(stderr,"capt: ccard[%d]:%d, gr:%d,\n",j,CAP(tb)->ccard[j],gr); 
    }
    return;
}

static inline void capt_tmbnk (psb_t *tb)
{   int j;
    unsigned char gr=0;
    
    CAP(tb)->tmbnk = 0;
    for (j=0; j<10; j++)
    { gr = crop_grey(tb, &(GMP(tb)->tmbnk[j]));
      if (gr > 70) CAP(tb)->tmbnk += 10;
      if ((Debug(tb))) 
        fprintf(stderr,"capt: tmbnk[%d]:%d, gr:%d,\n",j,CAP(tb)->tmbnk,gr); 
    }
    return;
}

static inline void capt_dealer (psb_t *tb)
{   unsigned char gr=0;
    int j;

    CAP(tb)->dealer = 0;
    for (j=1; j<=Seats(tb); j++)
    { gr = crop_grey(tb, &GMP(tb)->dealer[j]);
      if (gr > 50) CAP(tb)->dealer = j;
      if (Debug(tb)) 
        fprintf(stderr,"capt: dealer[%d]:%d, gr:%d,\n",j,CAP(tb)->dealer,gr); 
    }
    return;
}

#define CAPTstack0 48
//#define CAPTident0 24
static inline void capt_stack (psb_t *tb)
{   char st[CAPTstack0]={0};
    gopt_t opt={GOCRstackt}, opc={GOCRstackc}, *op=0;
    imag_t *im=0;
    int j, nl=0;
//gmtx_t gmi;
//gopt_t opi={GOCRident};

    if (!Flpnt(tb)) op=&opt; else op=&opc; 
    for (j=1; j<=Seats(tb); j++)
    {   //stack
	st[0] = '\0';
    	if (!j) im = crop_text(tb, &(GMP(tb)->stack[1]), TRANmystk);
	else    im = crop_text(tb, &(GMP(tb)->stack[j]), TRANstack);
	nl = gocr_call(op, im, st, CAPTstack0);
        remchars(st, ' ');
        if (st[0]) strncpy(CAP(tb)->stack[j], st, CAPTstack0); 
	else CAP(tb)->stack[j][0] = '\0';
    if (Debug(tb)) 
    fprintf(stderr, "capt: stack[%d]:%s, nl:%d,\n", j,CAP(tb)->stack[j],nl); 
	crop_free(im);
/*
	//ident
	st[0] = '\0';
        gmi = GMP(tb)->stack[j];
	gmi.y -= gmi.h;
	im = crop_text(tb, &gmi, TRANident); 
	nl = gocr_call(&opi, im, st, CAPTident0);
        remchars(st, ' ');
	if (st[0]) strncpy(CAP(tb)->ident[j], st, CAPTident0); 
	else CAP(tb)->ident[j][0] = '\0';
    if (Debug(tb)) 
    fprintf(stderr,"capt: ident[%d]:%s, nl:%d,\n", j,CAP(tb)->ident[j],nl);
	crop_free(im);
*/
    }
    return;
}

#define CAPTbetpr0 48
static inline void capt_betpr (psb_t *tb)
{   char st[CAPTbetpr0]={0};
    gopt_t opt={GOCRbetprt}, opc={GOCRbetprc}, *op=0;
    imag_t *im=0;
    int j, nl=0;
  
    if (!Flpnt(tb)) op=&opt; else op=&opc; 
    for (j=1; j<=Seats(tb); j++) 
    {   st[0] = '\0';
    	im = crop_text(tb, &(GMP(tb)->betpr[j]), TRANbetpr);
	nl = gocr_call(op, im, st, CAPTbetpr0);
        remchars(st, ' ');
        CAP(tb)->betpr[j] = (st[0])? atof(st) : 0; 
    if (Debug(tb)) 
    fprintf(stderr,"capt: betpr[%d]:%.2f, nl:%d,\n",j,CAP(tb)->betpr[j],nl);
	crop_free(im);
    }
    return;
}

static inline void capt_empty (psb_t *tb)
{   unsigned char gr=0;
    int j, k, tn[5]={3,1,0,0,0};
    gmtx_t gm;

    for (j=1; j<=Seats(tb); j++)
    {   gm = GMP(tb)->empty[j];
        gm.x += tn[3];  gm.y += tn[4];
        CAP(tb)->empty[j] = 1;
        for (k=0; k<tn[0]; k++)
        {   if (!CAP(tb)->empty[j]) break;
            gr = crop_grey(tb, &gm);
            CAP(tb)->empty[j] = (gr < 10);
        if (Debug(tb)) 
	fprintf(stderr,"capt: empty[%d]:%d, gr:%d,\n",j,CAP(tb)->empty[j],gr);
            gm.x += tn[1]; gm.y += tn[2];        
        }
    }
    return;
}

#define CAPTident0 24
static inline void capt_ident (psb_t *tb)
{   char st[CAPTident0]={0};
    imag_t *im=0;
    int j, nl=0;
    gopt_t op={GOCRident};

    for (j=1; j<=Seats(tb); j++)
    {   st[0] = '\0';
        im = crop_text(tb, &(GMP(tb)->ident[j]), TRANident); 
	nl = gocr_call(&op, im, st, CAPTident0);
        remchars(st, ' ');
	if (st[0]) strncpy(CAP(tb)->ident[j], st, CAPTident0); 
	else CAP(tb)->ident[j][0] = '\0';
    if (Debug(tb)) 
    fprintf(stderr,"capt: ident[%d]:%s, nl:%d,\n", j,CAP(tb)->ident[j],nl);
	crop_free(im);
    }
    return;
}

/*****************************************************************************/
int capt_capt0 (psb_t *tb) //psb_capt()
{   if (!tb) {error(0,0,"capt_capt0: null pointer"); return 0;}
    capt_tocll (tb);
    capt_torse (tb);
    capt_pot_t (tb);
    capt_pot_s (tb);
    capt_vcard (tb);
    //-------------/	
    capt_bfold (tb);
    capt_bcall (tb);
    capt_raise (tb);
    capt_bback (tb);
    capt_hftab (tb);
    capt_sftab (tb);
    capt_pchat (tb);
    capt_bobet (tb);
    //-------------/
    capt_pinfo (tb);
    if (CAP(tb)->tinfo)
    {   capt_place (tb);
        capt_paidp (tb);
        capt_blind (tb);
        capt_stave (tb);
    }	
    //capt_dealer(tb); //capt_stage()
    //capt_ccard (tb); //  
    //capt_tmbnk (tb); //capt_captt()
    //capt_betpr (tb); //capt_captb()
    //capt_stack (tb); //
    //capt_empty (tb); //capt_state()
    //capt_ident (tb); //
    CAP(tb)->yturn = CAP(tb)->bfold || CAP(tb)->raise;
    if (Debug(tb)) fprintf(stderr, "capt: yturn:%d,\n", CAP(tb)->yturn);
    return CAP(tb)->yturn;
}

int capt_state (psb_t *tb) //getstates()
{   if (!tb) {error(0,0,"capt_state: null pointer"); return 0;}
    capt_stack (tb);
    capt_empty (tb);
    capt_ident (tb);
    return 1;
}

int capt_stage (psb_t *tb) //findstage() and card's color 
{   if (!tb) {error(0,0,"capt_stage: null pointer"); return 0;}
    capt_ccard (tb); 
    capt_dealer(tb); 
    return 1;
}

int capt_captb (psb_t *tb) //getstates() getblinds()
{   if (!tb) {error(0,0,"capt_captb: null pointer"); return 0;}
    capt_betpr (tb); 				     return 1;
}
int capt_captt (psb_t *tb) //psb_wait()
{   if (!tb) {error(0,0,"capt_captt: null pointer"); return 0;}
    capt_tmbnk (tb); 				     return CAP(tb)->tmbnk;
}

int capt_captp (psb_t *tb) //psb_push()
{   if (!tb) {error(0,0,"capt_captp: null pointer"); return 0;}
    capt_bfold (tb);
    capt_bcall (tb);
    capt_raise (tb);
    capt_bback (tb);
    capt_hftab (tb);
    capt_sftab (tb);
    capt_pinfo (tb);
    capt_pchat (tb);
    capt_bobet (tb);
    CAP(tb)->yturn = CAP(tb)->bfold || CAP(tb)->raise;
    return CAP(tb)->yturn;
}

int capt_captw (psb_t *tb) //psb_wbox() 
{   if (!tb) {error(0,0,"capt_captw: null pointer"); return 0;}
    capt_pchat (tb);
    capt_bobet (tb);
    capt_stack (tb);
    return 1;
}

int capt_captu (psb_t *tb) //psb_turn()
{   if (!tb) {error(0,0,"capt_captu: null pointer"); return 0;}
    capt_bfold (tb);
    capt_raise (tb);
    CAP(tb)->yturn = CAP(tb)->bfold || CAP(tb)->raise;
    if (Debug(tb)) fprintf(stderr, "capt: yturn:%d,\n", CAP(tb)->yturn);
    return CAP(tb)->yturn;
}

int capt_save0 (psb_t *tb, gmtx_t *gm, float resize, int togrey) 
{   if (!tb || !gm) {error(0,0,"capt_save0: null pointers"); return 0;}
    char s0[48];
    XImage *xp=0;
    MagickWand *mp=0;
    int x=gm->x, y=gm->y, w=gm->w, h=gm->h;

    if (Debug(tb)>1)
    { fprintf(stderr, "capt: save0: win_w=%d win_h=%d display=%p window=%lx\n", 
          Win_w(tb), Win_h(tb), tb->cf.dpy, Winid(tb));
      fprintf(stderr, "capt: save0: f=%f,%f,%f,%f, g=%d,%d,%d,%d,\n", 
          gm->fg.x, gm->fg.y, gm->fg.w, gm->fg.h, x, y, w, h);
    }
    xp = XGetImage(tb->cf.dpy, Winid(tb), x,y,w,h, AllPlanes, ZPixmap);
    if (!xp) {error(0,0,"capt_save0: fail to get image from X"); return 0;}
    MagickWandGenesis();
    mp = NewMagickWand();
    if (!mp) error(0,0,"capt_save0: failed to create new MagickWand.");
    else
    {	MagickConstituteImage(mp, w, h, "BGRP", CharPixel, xp->data);
        if (resize) 
        {   w = w * resize; 
    	    h = h * resize;
	    MagickResizeImage(mp, w, h, TriangleFilter, 1);
        }
        if (togrey) MagickTransformImageColorspace(mp, GRAYColorspace);
        if (tb->cf.logdir)
	{   sprintf(s0, "%s/%s.png", tb->cf.logdir, (gm->id)?gm->id:"save0"); 
            MagickWriteImage(mp, s0);  
	}
    }
    XDestroyImage(xp);
    DestroyMagickWand(mp);
    MagickWandTerminus();
    return 1; 
}

int capt_gmtr0 (psb_t *tb) 
{   if (!tb) {error(0,0,"capt_gmtr0: null pointer"); return 0;}
    char s0[48];
    XImage *xp=0;
    MagickWand *mp=0;
    DrawingWand *dp=0;
    PixelWand *pp=0;
    int j, w=Win_w(tb), h=Win_h(tb);
    int n = (sizeof(gmtr_t)/sizeof(gmtx_t)); //total geometries in gmtr_t
    gmtx_t *gp = (gmtx_t*)GMP(tb);

    if (tb->cf.dbgdir) sprintf(s0, "%s/%s", tb->cf.dbgdir,"gmtr0.png"); 
    else return 0;
          
    xp = XGetImage(tb->cf.dpy, Winid(tb), 0,0,w,h, AllPlanes, ZPixmap);
    if (!xp) {error(0,0,"capt_gmtr0: fail to get image from X"); return 0;}
    MagickWandGenesis();
    dp = NewDrawingWand();
    pp = NewPixelWand();
    mp = NewMagickWand();
    if (mp && pp && dp) 
    {   MagickReadImage(mp,s0); 
 	MagickConstituteImage(mp,w,h,"BGRP",CharPixel,xp->data);
        MagickTransformImageColorspace(mp,GRAYColorspace);
        
	PixelSetColor(pp,"none");
        DrawSetFillColor(dp,pp);
        PixelSetColor(pp,"red");
        DrawSetStrokeColor(dp,pp);
        DrawSetStrokeWidth(dp,1);    
	DrawSetFontSize(dp,16); 
  	
	char *id, *none=" ";
        for (j=0; j<n; j++,gp++) 
        {   if (!gp->id) continue;
	    if ( strncmp(gp->id,"tmbnk"   ,5)) id = gp->id; else
	    if (!strncmp(gp->id,"tmbnk[9]",8)) id = gp->id; else id = none;
            if (!gp->w) 
            {   PixelSetColor(pp,"red"); DrawSetFillColor(dp,pp); 
                DrawPoint(dp, gp->x, gp->y); 
                PixelSetColor(pp,"none"); DrawSetFillColor(dp,pp); 
            } 
            else DrawRectangle(dp, gp->x, gp->y, gp->x+gp->w, gp->y+gp->h); 
	    PixelSetColor(pp,"green"); 
	    DrawSetFillColor(dp,pp); DrawSetStrokeColor(dp,pp); 
            DrawAnnotation(dp,gp->x+5,gp->y-5,(const unsigned char*)id); 
            PixelSetColor(pp,"red"); DrawSetStrokeColor(dp,pp); 
            PixelSetColor(pp,"none"); DrawSetFillColor(dp,pp);
        }
        MagickDrawImage(mp, dp);
        MagickWriteImage(mp, s0);  
    }
    else {error(0,0,"capt_gmtr0: fail creating MagickWand"); return 0;}

    XDestroyImage(xp);
    DestroyPixelWand(pp);
    DestroyDrawingWand(dp);
    DestroyMagickWand(mp);
    MagickWandTerminus();
    return 1; 
} 

/*****************************************************************************/

