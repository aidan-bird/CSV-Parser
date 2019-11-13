#ifndef CSV_H
#define CSV_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

enum CSVType {
    REAL,
    INTEGER,
    STRING,
};

typedef enum CSVType CSVType;
typedef struct CSVUnit CSVUnit;
typedef struct CSV CSV;

struct CSV {
    int csvUnitCount;
    CSVUnit *unitbuf;
    char *strings;
};

struct CSVUnit {
    CSVType type;
    union {
        double d;
        long long int ll;
        char *str;
    };
};

CSV *parseCSV(const char *path);
void printCSV(CSV *csv);

#ifdef __cplusplus
}
#endif
#endif
