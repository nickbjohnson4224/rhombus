/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef MATH_H
#define MATH_H

/* general constants *******************************************************/

#define INFINITY (1.0/0.0)
#define NAN (0.0/0.0)
#define HUGE_VAL INFINITY

/* general functions *******************************************************/

float       ceilf (float x);
double      ceil  (double x);
long double ceill (long double x);

float       floorf(float x);
double      floor (double x);
long double floorl(long double x);

float       fabsf (float x);
double      fabs  (double x);
long double fabsl (long double x);

float       modff (float x, float *iptr);
double      modf  (double x, double *iptr);
long double modfl (long double x, long double *iptr);

float       fmodf (float x, float div);
double      fmod  (double x, double div);
long double fmodl (long double x, long double div);

/* exponential and logarithmic constants ***********************************/

#define M_E        2.7182818284590452353602874713526625
#define M_SQRT2    1.4142135623730950488016887242096981
#define M_SQRT1_2  0.7071067811865475244008443621048490
#define M_LOG2E    1.4426950408889634073599246810018921
#define M_LOG10E   0.4342944819032518276511289189166051
#define M_LN2      0.6931471805599453094172321214581765
#define M_LN10     2.3025850929940456840179914546843642

/* exponential and logarithmic functions ***********************************/

float       powf  (float b, float e);
double      pow   (double b, double e);
long double powl  (long double b, long double e);

float       expf  (float x);
double      exp   (double x);
long double expl  (long double x);

float       logf  (float x);
double      log   (double x);
long double logl  (long double x);

float       log10f(float x);
double      log10 (double x);
long double log10l(long double x);

float       log2f (float x);
double      log2  (double x);
long double log2l (long double x);

float       sqrtf (float x);
double      sqrt  (double x);
long double sqrtl (long double x);

float       ldexpf(float x, int n);
double      ldexp (double x, int n);
long double ldexpl(long double x, int n);

float       frexpf(float x, int *exp);
double      frexp (double x, int *exp);
long double frexpl(long double x, int *exp);

/* trigonometric constants *************************************************/

#define M_PI       3.1415926535897932384626433832795029
#define M_PI_2     1.5707963267948966192313216916397514
#define M_PI_4     0.7853981633974483096156608458198757
#define M_1_PI     0.3183098861837906715377675267450287
#define M_2_PI     0.6366197723675813430755350534900574
#define M_2_SQRTPI 1.1283791670955125738961589031215452

/* trigonometric functions *************************************************/

float       sinf  (float x);
double      sin   (double x);
long double sinl  (long double x);

float       cosf  (float x);
double      cos   (double x);
long double cosl  (long double x);

float       tanf  (float x);
double      tan   (double x);
long double tanl  (long double x);

float       asinf (float x);
double      asin  (double x);
long double asinl (long double x);

float       acosf (float x);
double      acos  (double x);
long double acosl (long double x);

float       atanf (float x);
double      atan  (double x);
long double atanl (long double x);

float       atan2f(float y, float x);
double      atan2 (double y, double x);
long double atan2l(long double y, long double x);

float       sinhf (float x);
double      sinh  (double x);
long double sinhl (long double x);

float       coshf (float x);
double      cosh  (double x);
long double coshl (long double x);

float       tanhf (float x);
double      tanh  (double x);
long double tanhl (long double x);

#endif/*MATH_H*/
