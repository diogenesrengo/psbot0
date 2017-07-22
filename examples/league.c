
#include <psbot0.h>

//----------------------------------------------------------------------------/
int main (int argc, char **argv) 
{   char s0[80]={0};
    int r=0; // j, x;
    
#define MAXPHR 20
    char *phrase[ MAXPHR ] = 
    {   "really nh!",
        "good game!",
        "lucky guy",
        "gotta go!",
        "go to walk",
	"go to sleep",
	"I'd wish to see some flop!",
	"take care!",
	"sorry bro",
	"you donkey",
	"never mind!",
	"excelent!!",
	"come on bb",
	"","","","","","","",
    }; 
    srand(time(0));
#define ranp /*type a random phrase*/ do \
{   sprintf(s0, "%s", phrase[(rand() % MAXPHR)]); \
    psb_wbox(tb,PSB_WRITECHAT,s0,0); sleep(1); psb_push(tb,PSB_PAGEINFO); \
} while(0)
#define prob(p) ((rand()%100) <= p) /*eg: prob(50) means 50% of times*/
            
    psb_t *tb = psb_psb0();
    //----------------------------------------/
    tb->tmbnk=80;
    //tb->pp_ra=CO; tb->pp_po=SB; 
    //tb->bl_bb=100; tb->bl_sb=50;
    tb->cf.seats=9;
    tb->cf.flpnt=0; //tourney;
    tb->cf.wsize=0; 
    tb->cf.sinbb=1; 
    tb->cf.shwcf=0; 
    tb->cf.logtb=1; 
    tb->cf.debug=0; 
    //strncpy(tb->cf.dbgdir,"./psbot0-dbg",80);
    //strncpy(tb->cf.logdir,"./psbot0-log",80);
    //----------------------------------------/
    psb_args(tb,argc,argv);
    psb_init(tb,0,0); 
    ranp;
    
    noy_t cjj, cqq, ckk, ctt, c__, cq8, cxx;
    noy_t b20, b16, b12 /*,b10*/;
    noy_t s06, s09, s12, s02, s01;

    while (psb_loop(tb,4))
    {
    r = 0;

    c__ = psb_prcs(tb,"99-"); //trash
    s01 = (tb->mystk < 1);
    if (tb->stage==PSBSTG_PR)
    {	ckk = psb_prcs(tb,"KK+");
	cqq = psb_prcs(tb,"QQ+");
        cjj = psb_prcs(tb,"JJ+"); 
	ctt = psb_prcs(tb,"TT+");
	cq8 = psb_prcs(tb,"Q8+");
	cxx = psb_prcs(tb,"22p"); //pairs
	b20 = (tb->bl_bb > (200-1));
	b16 = (tb->bl_bb > (160-1));
	b12 = (tb->bl_bb > (120-1));
	//b10 = (tb->bl_bb > (100-1));
	s06 = (tb->mystk < 6);
	s09 = (tb->mystk < 9);
	s12 = (tb->mystk <12);
	s02 = (tb->mystk < 2);
	psb_info(tb,"@ b20:%d%d%d b16:%d%d%d b12:%d%d%d s06:%d%d s02:%d%d\n",
	         b20,s06,cjj, b16,s09,cqq, b12,s12,ckk, s06,ckk, s02,ctt);
	if ((b20 && s06 && cjj) 
	 || (b16 && s09 && cqq) 
	 || (b12 && s12 && ckk) 
	 || (       s06 && ckk) 
	 || (       s02 && ctt) 
	 || (       s09 && ckk)) r=psb_mrse(tb,"a",0); 
	else if (tb->plsin == 0) r=psb_mrse(tb,"b",2); //where did people go?
	else if (s09 && !tb->plsyt && psb_prpo(tb,PSBPOS_BU,0) && cq8)
	    r=psb_mrse(tb,"b",5); //pushing it ;)
        else if (cxx && !tb->rsepr && tb->mystk>10) r=psb_mcll(tb); //try to set
	switch (tb->posit)
	{ case PSBPOS_SB: if (s01 || (!tb->rsepr && cjj)) r=psb_mcll(tb); break;
	  case PSBPOS_BB: if (s01) r=psb_mcll(tb); break;
	  default: ;
        }
    } 
    else if (tb->hrank >= PSBRNK_3K) 
    {	switch (tb->stage)
	{ case PSBSTG_FL: r=psb_mrse(tb,"p",40); break;
	  case PSBSTG_TU: if (prob(50)) r=psb_mrse(tb,"p",30); break;
	  case PSBSTG_RI: if (prob(20)) r=psb_mrse(tb,"p",200); else
			  if (prob(50)) r=psb_mrse(tb,"p",30); break;
	  default : ;
	}
	if (!r) r=psb_mcll(tb);
    } 
    else if ((s01 && !c__) /*|| (tb->rdraw && tb->tocll<2)*/) r=psb_mcll(tb);
    else if (tb->stage==PSBSTG_FL && !tb->sg.co && tb->check && tb->plsin<=2)
	r=psb_mrse(tb,"p",30); 
	       
    if (!r) psb_mfld(tb);
    if (!tb->tinfo) psb_push(tb,PSB_PAGEINFO); //whatever happens, we need it
	
    if ((tb->stage==PSBSTG_RI) || (!(tb->round%5) && tb->stage==PSBSTG_PR))
        ranp;
    }

    psb_free (tb);
    return 0; 
}

//----------------------------------------------------------------------------/

