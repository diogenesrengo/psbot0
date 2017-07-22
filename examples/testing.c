
#include <psbot0.h>

//----------------------------------------------------------------------------/
#define NEXT_CAPT   sprintf(s,"xdotool key --window %lu space",wid); system(s)
#define ACTTERMINAL sprintf(s,"xdotool windowactivate %lu",wit); system(s)
#define IDISPLAYWID \
  FILE *p1=popen("xwininfo -tree -root |grep 'ImageMagick' |cut -d' ' -f9","r");\
  fgets(s, sizeof(s), p1); \
  pclose(p1); \
  wid = parseWid(s)    
    
int main (int argc, char **argv) 
{   char s[80]={0};
    Window wid=0, wit=0;
    psb_t *tb;

    printf("# testing with capts in Magick display\n");
    wit = parsewid(getenv("WINDOWID"));

    tb = psb_psb0();
    psb_args(tb,argc,argv);
    psb_init(tb,0,0); wid = tb->cf.win;
    //IDISPLAYWID; psb_init(tb,wid,9);
    psb_actw(tb,2);
    
    while (1)
    {	psb_capt(tb);
	psb_show(tb);
	ACTTERMINAL; 
	printf("\n@ enter to go next capt");
        getchar();
        printf("@ wait...\n");
	NEXT_CAPT; 
	psb_actw(tb,3);
    }	
    psb_free(tb);
    
    return 0; 
}

//----------------------------------------------------------------------------/

