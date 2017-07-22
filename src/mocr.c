
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pgm2asc.h"
#include "mocr.h"

/*****************************************************************************/
job_t *OCR_JOB;

int gocr_call (gopt_t *opts, imag_t *im, char *buff, int lenght) 
{   if (!im || !im->p || !buff) return 0; 
	
    // init gocr job structures 
    job_t job0, *job; 
    job = OCR_JOB = &job0;
    job_init(job);
    job_init_image(job);

    // set options, default gopts={0} 
    job->cfg.cfilter      =    opts->C;
    job->cfg.unrec_marker = ( !opts->u )? "_": opts->u; 
    job->cfg.lc           = ( !opts->c )? "_": opts->c; 
    job->cfg.dust_size    = ( !opts->d )?  -1: opts->d; 
    job->cfg.spc          =    opts->s;
    job->cfg.certainty    = ( !opts->a )?  95: opts->a;
    job->cfg.verbose      =    opts->v;
    
    job->cfg.cs = 127; //no threshold 

    // set image data 
    job->src.p.x = im->x; 
    job->src.p.y = im->y; 
    job->src.p.bpp = 1; //bytes per pixel, always 1 
    job->src.p.p = im->p; 
    
    //-----------------------------------------------------------------------/
    // pgm2asc() function is touched, not original from gocr 
    // nor otsu() nor thresholding() 
    // nor detect_barcode()
    // nor detect_pictures() nor remove_pictures()
    // nor detect_rotation_angle()
    //-----------------------------------------------------------------------/
    pgm2asc(job);

    // number of recognized lines, maybe useful
    int nl = job->res.linelist.n, k=0;
    
    // return first not null recognized line 
    do 
    {   char *resp = (char*) getTextLine( &(job->res.linelist), k++);
        if (resp && (resp[0] != '\0'))
            strncpy(buff, resp, lenght);
        else 
            buff[0] = '\0';
    } while (k < nl && buff[0]=='\0');

    // my free job structure 
    if (job->tmp.ppo.p==job->src.p.p) job->tmp.ppo.p=NULL;
    list_and_data_free(&(job->res.boxlist), (void (*)(void *))free_box);
    //if (job->src.p.p) { free(job->src.p.p); job->src.p.p=NULL; }
    if (job->tmp.ppo.p) { free(job->tmp.ppo.p); job->tmp.ppo.p=NULL; }
    free_textlines(&(job->res.linelist));
	
    return nl;
}

/*****************************************************************************/

