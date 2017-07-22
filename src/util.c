
#include <string.h>

/* global function declarations. *********************************************/
int remchars (char *st, char ch); //remove chars 
int splittok (char *st, char *ts[], char *dl); //split in tokens, dl:delimiters
int rotarray (int *arr, int len, int rot); //rotate array
int srtarray (int *arr, int len); //sort array, low to high

/*****************************************************************************/
int remchars (char *st, char ch)
{   int n=0;
    char *i=st, *j=st;
    if (!st) return 0;
    while (*j != 0) 
    {   *i= *j++;
    	if (*i != ch) i++, n++; 
    }
    *i= 0;
    return n; /* lenght of string */
}

int splittok (char *str, char *tok[], char *del)
{   int n=0;
    if (!str || !del) return 0; 
    tok[n] = strtok(str, del);
    while (tok[n] != 0) tok[++n] = strtok(0, del); 
    return n; /* number of tokens */
}

int rotarray (int *arr, int len, int rot)
{   int i, j, cnt=0, idx=0, tmp, prv=arr[idx];
    //if (rot<0 || rot>len || !arr) return 0;
    if (rot<0 && rot>(-1*len)) rot = len + rot; 
    if (rot>len || !arr) return 0;
    for (i=0; i<len; i++) 
    {	j = (idx + rot) % len;
    	tmp = arr[j];
    	arr[j] = prv;
    	prv = tmp;
    	idx = j;
    	cnt += rot;
    	if ((cnt % len) == 0) 
    	{   cnt = 0;
    	    idx = (idx + 1) % len;
    	    prv = arr[idx];
    	}
    }
    return 1;
}  

int srtarray (int *arr, int len) 
{   int i, j;  
    int ax;       
    if (!arr || len<0) return 0;
    for (i=0; i<len-1; i++) 
        for (j=i+1; j<len; j++)     
	    if (arr[i] > arr[j])
            {	ax = arr[i];
            	arr[i] = arr[j];
            	arr[j] = ax;
            }
    return 1;
}

/*****************************************************************************/

