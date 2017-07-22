
#ifndef PSB_GOCR_H
#define PSB_GOCR_H 1

/*****************************************************************************/
typedef struct //gocr options, same gocr program - see help 
{   char *C; //( 0 ) chars filter, 0 for all chars 
    char *u; //("_") output marker for unrecognized chars 
    char *c; //("_") list of chars not recognized  
    int d;   //( 0 ) spacewidth/dots, 0 for autodetect 
    int s;   //(-1 ) dust size 
    int a;   //(95 ) set certainty 
    int v;   //( 0 ) verbosity, bitmap 1 2 4 8 16 32 
} gopt_t;

typedef struct //image data 
{   int x; 	      //width 
    int y; 	      //height 
    unsigned char *p; //buffer
} imag_t;

/* 
 * recognize text in image, only 1 byte per pixel images in greyscale.
 * arg:
 *     opts - gocr options in gopt_t struct
 *       im - image data in imag_t struct
 *   buffer - where return first line of recognized text 
 *   lenght - lenght of buffer 
 * ret:
 *   number of recognized text lines 
 */
int gocr_call (gopt_t *opts, imag_t *im, char *buffer, int lenght); 

/*****************************************************************************/
#endif 

