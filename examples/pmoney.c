
#include <psbot0.h>

//----------------------------------------------------------------------------/
int main (int argc, char **argv) 
{   char s0[80]={0};
    int r=0; // j, x;
    
#define MAXPHR 16
    char *phrase[ MAXPHR ] = 
    {   "really nh!",
        "good game!",
        "lucky guy",
        "gotta go!",
        "go to walk",
	"go to sleep",
	"fkc fisher!",
	"take care!",
	"sorry bro",
	"you donkey",
	"never mind!",
	"excelent!!",
	"come on bb",
	"","","", //"","","","",
    }; 
    srand(time(0));
#define RANPHR /*type a random phrase*/ do \
{   sprintf(s0, "%s", phrase[(rand() % MAXPHR)]); \
    psb_wbox(tb,PSB_WRITECHAT,s0,0); sleep(1); psb_push(tb,PSB_PAGEINFO); \
} while(0)
#define prob(p) ((rand()%100) <= p) /*eg: prob(50) means 50% of times*/
            
    psb_t *tb = psb_psb0();
    //----------------------------------------/
    //tb->tmbnk=80;
    //tb->pp_ra=CO; tb->pp_po=SB; 
    tb->bl_bb=100; tb->bl_sb=50;
    tb->cf.seats=0; //default: 9
    tb->cf.flpnt=0; //there's float point
    tb->cf.wsize=0; 
    tb->cf.sinbb=0; 
    tb->cf.shwcf=0; 
    tb->cf.logtb=0; 
    tb->cf.debug=0; 
    //strncpy(tb->cf.dbgdir,"./psbot0-dbg",80);
    //strncpy(tb->cf.logdir,"./psbot0-log",80);
    //----------------------------------------/
    psb_args(tb,argc,argv);
    psb_init(tb,0,0); 
    RANPHR;
/*    
  while (1)
  {
  psb_actw (tb,4);
  if (psb_turn(tb))
  { psb_capt (tb);
    psb_show (tb);
*/
  while (psb_loop(tb,4))
  { 
    r = 0;
    if (tb->stage==PSBSTG_PR)
    {	switch (tb->posit)
	{ case PSBPOS_UT: //if (psb_prcs(tb,"KK+") r=psb_mrse(tb,"b",3); break;
	  case PSBPOS_U1:  
	  case PSBPOS_M4:  
	  case PSBPOS_M3:  
	  case PSBPOS_M2:  
	  case PSBPOS_M1:
	  case PSBPOS_CO:
	  case PSBPOS_BU:  
	  case PSBPOS_SB: 
	  case PSBPOS_BB: //if (psb_prcs(tb,"22+p01,JJ+")) r=psb_mrse(tb,"b",2);
	      		  if (tb->tocll < 500) r=psb_mcll(tb);
			  break;
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
    else if (tb->stage==PSBSTG_FL && !tb->sg.co && tb->check && tb->plsin<=2)
	r=psb_mrse(tb,"p",30);
	       
    if (!r) psb_mfld(tb);
	
    if ((tb->stage==PSBSTG_RI) || (!(tb->round%5) && tb->stage==PSBSTG_PR))
        RANPHR;
  //}
  }
    psb_free (tb);
    return 0; 
}

//----------------------------------------------------------------------------/

