#pragma once
#include <lib/stdint.h>

int abs(int i);
int clamp(int i, int min, int max);
double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double x, double y);
double cos(double x);
double cosh(double x);
double sin(double x);
double sinh(double x);
double tanh(double x);
double exp(double x);
double frexp(double x, int* exponent);
double ldexp(double x, int exponent);
double log(double x);
double log10(double x);
double modf(double x, double* integer);
double pow(double x, int y);
double sqrt(double x);
double ceil(double x);
double fabs(double x);
double floor(double x);
double fmod(double x, double y);