#ifndef REGEX_H
#define REGEX_H

#include "string.h"

// Supports only '^', '$', '*', '|' and '.'
int regex_match(char *text, char *pattern)
{
	int fails = 0, p_len = strlen(pattern), t_len = strlen(text);
	char *reg_str = pattern, *text_str = text;
	for(int i = 0; i < t_len; i++)
	{
		fails = 0;
		for(int j = 0; j < p_len; j++)
		{
			reg_str = &pattern[j], text_str = &text[i];
			for(;;)
    		{
    			if((*reg_str == '^') && (reg_str == &pattern[0]) && (text_str == &text[0]))
    			{
    				if((*(reg_str+1) != *text_str) && (*(reg_str+1) != '.')) 
        				fails++;
        	    	reg_str += 2, text_str++;
    			}
    			else if((*(reg_str+1) == '$') && ((reg_str+1) == &pattern[p_len-1]) && (text_str == &text[t_len-1]))
    			{
    				if((*reg_str != *text_str) && (*reg_str != '.')) 
        				fails++;
    				reg_str += 2, text_str++;
    			}
    			else if(*(reg_str+1) == '|')
    			{
    				if((*reg_str != *text_str) && (*text_str != *(reg_str+2)) && (*reg_str != '.' && *(reg_str+2) != '.'))
    					fails++;
    				reg_str += 3, text_str++;
    			}
    			else if(*(reg_str+1) == '*') 
    			{ 	
    				while((*text_str == *reg_str || *reg_str == '.') && (*text_str != *(reg_str+2)) && (*text_str != '\0'))
    					text_str++;	
    				reg_str += 2;	
    			}
        		else if(*reg_str && *text_str)
        		{
        			if((*reg_str != *text_str) && (*reg_str != '.')) 
        				fails++;
        			reg_str++, text_str++;
        		}
        		else
        		{
        			if(*reg_str)
        				fails++;
            		if(fails)
            			break;
            		return 1; 
        		}
        	}
    	}
	}
	if(fails || *reg_str)
		return 0;
	return 1; 
}


#endif