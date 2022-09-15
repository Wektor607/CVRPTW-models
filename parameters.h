#pragma once
// for towns
//#define countTowns 21

#define speed 60.0 //km/h
#define TimeToDist 3600.0 * speed //
#define KmToSeconds (3600.0 / speed) // перевод км в секунды с учетом средней скорости

// for cars
//#define maxCapacity 30
long long maxCountCar =  3;
double optimalRadius = 0.37;
//#define countFiles 1


// for lkh
#define ALGFOR(i) i < countUpdate

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG_LOG(...) \
	do { \
		if (DEBUG) \
			printf("[%s]:%d:%s(): ", __FILENAME__, __LINE__, __func__); \
			printf(__VA_ARGS__); \
	} while (0)


//lkh3optTw
#define LKH lkh2opt
char fileout[] = "LKH_2opt_cvrp_result_100.txt";
// int maxCapacity = 50;
//for sa
long double tmax = 1000;
long double tmin = 1;

//parameters-bin
long long countTowns = 101;
#define countFilesBin 18

#define pathSavingTowns "bin-towns"
#define noteFile "test"

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define BLACK   "\033[37;40m"
#define YELLOW  "\033[1;33m"
#define WHITE   "\033[37;46m"
