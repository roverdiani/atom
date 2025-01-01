#ifndef VECTORS_H
#define VECTORS_H

#include "defines.h"

static int spvec __attribute__ ((section (".spvec"))) __attribute__ ((__used__)) = ADDRESS_STACK_POINTER_VEC;
static int pcvec __attribute__ ((section (".pcvec"))) __attribute__ ((__used__)) = ADDRESS_PROGRAM_COUNTER_VEC;

#endif // VECTORS_H