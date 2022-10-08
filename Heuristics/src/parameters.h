#pragma once

#define speed 60.0 //km/h
#define TimeToDist 3600.0 * speed //
#define KmToSeconds (3600.0 / speed) // перевод км в секунды с учетом средней скорости

//for sa
// long double tmax = 1000;
// long double tmin = 0.1;

//parameters-bin
long long countTowns = 101;
#define countFilesBin 18

#define pathSavingTowns "bin-towns"

