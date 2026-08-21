#ifndef _STD_BOOL_H
#define _STD_BOOL_H

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
typedef _Bool bool;
#define true 1
#define false 0
#endif

#endif