#ifdef __cplusplus
extern "C"
{
#endif

#include "csv.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
readfile(char **buf, const char *path)
{
    FILE *fp;
    int ret;

    fp = fopen(path, "rb");
    if (!fp)
        goto error1;
    fseek(fp, 0, SEEK_END);
    ret = ftell(fp);
    rewind(fp);
    if (!(*buf = (char *)malloc(ret + 1)))
        goto error2;
    fread(*buf, 1, ret, fp);
    (*buf)[ret] = '\0';
    fclose(fp);
    return ret;
error2:
    fclose(fp);
error1:
    return -1;
}

int
sumCharOccurrences(int c, int n, const char *str)
{
    int ret;

    ret = 0;
    for (int i = 0; i < n; i++)
        if (c == str[i])
            ret++;
    return ret;
}

char *
nextSeparator(char *str, char separator)
{
    while (*str) {
        if (*str == '\\') {
            str++;
            if (!(*str))
                return NULL;
        }
        if (*str == separator)
            break;
        str++;
    }
    return str;
}

int
parseNumber(char *sr, CSVUnit *unit, char* srend)
{
    /* check if sr is a valid number */  
    size_t i;
    char *bounds;
    int hasDeciPoint;
    char tmp;

    /* trim whitespace */  
    bounds = srend;
    while (*bounds <= ' ') {
        if (bounds == sr)
            return -1;
        bounds--;
    }
    if (bounds == sr) {
        unit->ll = *bounds - '0';
        unit->type = INTEGER;
        return 0;
    }
    i = 0;
    hasDeciPoint = 0;
    if (*sr == '+' || *sr == '-')
        i++;
    while (sr + i <= bounds) {
        if (sr[i] == '.') {
            if (hasDeciPoint)
                return -1;
            hasDeciPoint = 1;
        } else if (sr[i] > '9' || sr[i] < '0')
            return -1;
        i++;
    }
    tmp = *(bounds + 1);
    *(bounds + 1) = '\0';
    errno = 0;
    if (!hasDeciPoint) {
        unit->ll = atoi(sr);
        unit->type = INTEGER;
    } else {
        unit->d = atof(sr);
        unit->type = REAL;
    }
    if (errno)
        return -1;
    *(bounds + 1) = tmp;
    return 0;
}

char *
parseString(char *sr, CSVUnit *unit, char *stringbuf)
{
    if (*sr != '"')
        return NULL;
    sr++;
    unit->type = STRING;
    unit->str = stringbuf;
    while (*sr) {
        if (*sr == '"')
            break;
        if (*sr == '\\') {
            sr++;
            if (!(*sr))
                return NULL;
        }
        *stringbuf = *sr;
        sr++;
        stringbuf++;
    }
    *stringbuf = '\0';
    return stringbuf + 1;
}

char *
nextNonWhitespace(char *str)
{
    while (*str && *str <= ' ')
        str++;
    return str;
}

CSV *
parseCSV(const char *path)
{
    int fsize;
    int csvUnitBufCount;
    char substrEndState;
    char *fbuf;
    char *sr;
    char *substrend;
    char *stringwritter;
    char *stringwrittertmp;
    CSV *ret;

    if ((fsize = readfile(&fbuf, path)) < 0)
        goto error1;
    if (!fsize)
        goto error2;
    csvUnitBufCount = sumCharOccurrences(',', fsize, fbuf) + 1;
    if (!(ret = (CSV *)malloc(sizeof(CSV) + sizeof(CSVUnit) * csvUnitBufCount + fsize * 2)))
        goto error2;
    ret->unitbuf = (CSVUnit *)((void *)ret + sizeof(CSV));
    ret->strings = (char *)((void *)ret + sizeof(CSV) + sizeof(CSVUnit) * csvUnitBufCount);
    ret->csvUnitCount = 0;
    stringwritter = ret->strings;
    sr = fbuf;
    for (int i = 0; i < csvUnitBufCount; i++) {
        if (!(*sr))
            break;
        sr = nextNonWhitespace(sr);
        if (!sr)
            goto error3;
        substrend = nextSeparator(sr, ',');
        if (!substrend)
            goto error3;
        substrEndState = *substrend;
        *substrend = '\0';
        stringwrittertmp = parseString(sr, ret->unitbuf + i, stringwritter);
        if (stringwrittertmp)
            stringwritter = stringwrittertmp;
        else if (parseNumber(sr, ret->unitbuf + i, substrend))
            goto error3;
        *substrend = substrEndState;
        sr = substrend + 1;
        ret->csvUnitCount++;
    }
    free(fbuf);
    return ret;
error3:;
    free(ret);
error2:;
    free(fbuf);
error1:;
    return NULL;
}

void
printCSV(CSV *csv)
{
    for (int i = 0; i < csv->csvUnitCount; i++) {
        switch (csv->unitbuf[i].type) {
            case REAL:
                printf("%d: %f", i, csv->unitbuf[i].d);
                break;
            case INTEGER:
                printf("%d: %lld", i, csv->unitbuf[i].ll);
                break;
            case STRING:
                printf("%d: %s", i, csv->unitbuf[i].str);
                break;
        }
        putchar('\n');
    }
}

#ifdef __cplusplus
}
#endif
