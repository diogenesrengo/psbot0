
#ifndef PSB_UTIL_H
#define PSB_UTIL_H 1

/* global function declarations. *********************************************/
int remchars (char *st, char ch); //remove chars 
int splittok (char *st, char *ts[], char *dl); //split in tokens, dl:delimiters
int rotarray (int *arr, int len, int rot); //rotate array
int srtarray (int *arr, int len); //sort array, low to high

/*****************************************************************************/
#endif 

