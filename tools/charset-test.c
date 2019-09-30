
#include "tools-config.h"
#include "tools/charset.h"

void print08(char* title, char* p)
{
    int i;
    unsigned char* dt   = p;
    unsigned int   mask = 0x000000ff;
    printf("%-10s: ", title);
    for (i=0 ; dt[i]!=0 ; i++)
        printf(" %08x", dt[i]&mask);
    printf(" %08x\n", dt[i]&mask);
}

void print32(char* title, char* p, int len)
{
    int i;
    unsigned int* dt   = (unsigned int*)p;
    unsigned int   mask = 0xffffffff;
    printf("%-10s: ", title);
    for (i=0 ; dt[i]!=0 ; i++)
        printf(" %08x", dt[i]&mask);
    printf(" %08x ; i/len=%d/%d\n", dt[i]&mask,i+1,len);
}


int main()
{
    char* org = "azerty";
    printf( "%-10s: %s\n","org",org);
    print08("org", org);

    int      len   = strlen(org);
    Tuint32* uni32 = cs_conv_Utf8toUtf32(org,&len);
    print32("uni32", (char*)uni32 , len);

    char* orgrev = cs_conv_Utf32toUtf8(uni32,len);
    printf( "%-10s: %s\n","orgrev",orgrev);
    print08("orgrev", orgrev);

    char* zdb = cs_conv_Zdb8toUtf8(org);
    printf( "%-10s: %s\n","zdb",zdb);
    print08("zdb", zdb);

    len   = strlen(zdb);
    Tuint32* zdb32 = cs_conv_Utf8toUtf32(zdb,&len);
    print32("zdb32", (char*)zdb32 , len);


    char* zdbrev = cs_conv_Utf8toZdb8(zdb);
    printf( "%-10s: %s\n","zdbrev",zdbrev);
    print08("zdbrev", zdbrev);

    return 0;
}
