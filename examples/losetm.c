
#include <psbot0.h>

//lose time-------------------------------------------------------------------/
//  Wait a banktime percent then push fold button or ftab presel
//----------------------------------------------------------------------------/    
int main (int argc, char **argv) 
{   if (argc<3) 
    {   printf("# usage: %s -w [window id(0:mouse select)] -t [%% banktime]\n", 
	    argv[0]);
        exit(1);
    }
    
    psb_t *tb = psb_psb0();
    psb_args (tb,argc,argv);
    psb_init (tb,0,0);
    psb_actw (tb,2); 
    psb_wbox (tb,PSB_WRITECHAT,"hi everyone!",0);
    
    while (1)
    {   psb_actw(tb,4);
	if (psb_turn(tb)) { psb_wait(tb,tb->tmbnk); psb_push(tb,PSB_FOLDANY); }
    }
    
    psb_free (tb);

    return 0; 
}

//----------------------------------------------------------------------------/

