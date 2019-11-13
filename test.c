#include "csv.h"

int
main(int argc, char **argv)
{
    CSV *test;

    if (argc <= 1)
        return -1;
    test = parseCSV(argv[1]);
    if (!test)
        return -1;
    return 0;
}

