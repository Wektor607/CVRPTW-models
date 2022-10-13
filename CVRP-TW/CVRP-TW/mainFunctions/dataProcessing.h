#include "townActions.h"

#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

int read_file_tw(const char* name_file, twtown *towns, int counttowns);

void parseOneTwTownNoIndex(const char pathFile[], const char newFileName[], int tcountTowns);

void readOneTwTownByBinaryNoIndex(twtown *towns, halfmatrix *m, const char newFileName[]);

#endif