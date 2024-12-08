#ifndef VECTORS_H
#define VECTORS_H

static int spvec __attribute__ ((section (".spvec"))) __attribute__ ((__used__)) = 0x00010000;
static int pcvec __attribute__ ((section (".pcvec"))) __attribute__ ((__used__)) = 0x000F0008;

#endif // VECTORS_H