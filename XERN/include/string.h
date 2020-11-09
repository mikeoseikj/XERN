#ifndef STRING_H
#define STRING_H

int strlen(char *s)
{
    int len = 0;
    while(*s)
        s++, len++;

    return len;
}

char *strcpy(char *dest, char *src)
{
    int i;
    for(i = 0; src[i] != 0; i++)
        dest[i] = src[i];

    dest[i] = 0;
    return dest;
}

char *strncpy(char *dest, char *src, unsigned int size)
{  
    int i;
    for(i = 0; (i < size) && (src[i] != 0); i++)
        dest[i] = src[i];

    dest[i] = 0;    
    return dest;
}


int strcmp(char *s1, char *s2)
{
    while(1)
    {
        if((*s1 == *s2) && (*s1 && *s2))
        {
            s1++;
            s2++; 
        }
        else
        {
            if(*s1 > *s2)
                return 1;
            else if(*s1 < *s2)
                return -1;
            else 
                return 0;
        }
    }
}

int strncmp(char *s1, char *s2, unsigned int size)
{
    while(1)
    {
        if((*s1 == *s2) && (*s1 && *s2) && size)
        {
            size = size-1;
            s1++, s2++;
        }
        else
        {
            
            if(*s1 > *s2)
                return 1;
            else if(*s1 < *s2)
                return -1;
            else 
                return 0;
        }
    }
}


char *memcpy(char *dest, char *src, unsigned int size)
{
    for(int i = 0; i < size; i++)
        dest[i] = src[i];

    return dest;
}

char *memmove(char *dest, char *src, unsigned int size)
{
    char temp[size];
    for(int i = 0; i < size; i++)
        temp[i] = src[i];
  
    for(int i = 0; i < size; i++)
        dest[i] = temp[i];

    return dest;
}


char *memset(char *s, int c, unsigned int size) 
{    
    for(int i = 0; i < size; i++)
        s[i] = (char)c;

    return s;
}

int memcmp(char *s1, char *s2, unsigned int size) 
{
    for(int i = 0; i < size; i++)
    {
        if(s1[i] != s2[i])
            return s1[i] - s2[i];
    }
    
    return 0;
}

void reverse(char str[], char output[])
{
    int len = strlen(str);
    int pos = 0;

    for(int i = len-1; i >= 0; i--)
    {
        output[pos] = str[i];
        pos++;
    }
    output[pos] = 0;

}
char *strstr(char *haystack, char *needle)
{
    if(strlen(needle) > strlen(haystack))
        return 0;

    for(int i = 0; i < strlen(haystack); i++)
    {
        if(memcmp((char *)&haystack[i], needle, strlen(needle)) == 0)
            return &haystack[i];
    }
    return 0;
}

int atoi(char *s)
{
    int value = 0;
    while(*s)
    {
        value = 10 * value;
        value += (*s - '0');
        s++;
    }
    return value;
}

int readline(int fd, char buf[], int len)
{
    char c;
    int ret;
    for(int i = 0; i < len; i++)
    {
        ret = kread(fd, &c, 1);
        if(ret == 0 && i > 0)  // EOF
        {
            buf[i] = 0;
            return i;
        }
        else if(ret == 0)  // No data to read
            return -1;

        else if(ret < 0) // An error occurred
            return -2;

        if(c == '\n')
        {
            buf[i] = 0;
            return i;
        }
        buf[i] = c;
    }
    return -2;
}


#endif

