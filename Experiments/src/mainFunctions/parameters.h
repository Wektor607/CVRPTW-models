#pragma once

#define speed 60.0 //km/h
#define TimeToDist 3600.0 * speed //
#define KmToSeconds 3600.0 / speed // перевод км в секунды с учетом средней скорости
#define errortwtown maketwtown(errortown, -1, -1, -1)
#define MIN(a, b) ((a < b) ? (a) : (b))

