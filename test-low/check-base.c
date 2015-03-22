/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "entropy.h"                    // Wrapper around /dev/random

XX_PREDECLS

int main(int argc, char** argv)
{
    // Read command-line options
     
    int rounds = 5;
    bool nondeterministic_seed = false;
    
    ++argv; --argc;
    if (argc > 0 && strcmp(argv[0], "-r") == 0) {
         nondeterministic_seed = true;
         ++argv; --argc;
    }
    if (argc > 0) {
         rounds = atoi(argv[0]);
    }
    
    // In this version of the code, we'll use a local rng, rather than the
    // global one.
    
    XX_RAND_DECL
    
    // You should *always* seed the RNG.  The usual time to do it is the
    // point in time when you create RNG (typically at the beginning of the
    // program).
    //
    // XX_SRANDOM_R takes two YY-bit constants (the initial state, and the
    // rng sequence selector; rngs with different sequence selectors will
    // *never* have random sequences that coincide, at all) - the code below 
    // shows three possible ways to do so.

    if (nondeterministic_seed) {
        // Seed with external entropy
        
        XX_SEEDSDECL(seeds)
        entropy_getbytes((void*) seeds, sizeof(seeds)); 
        XX_SRANDOM(XX_SRANDOM_SEEDARGS(seeds));
    } else {
        // Seed with a fixed constant
        
        XX_SRANDOM(XX_SRANDOM_SEEDCONSTS);
    }
    
    printf(XX_INFO);
 
    for (int round = 1; round <= rounds; ++round) {
        printf("Round %d:\n", round);

        /* Make some XX-bit numbers */
        printf(XX_NUMBITS);
        for (int i = 0; i < XX_NUMVALUES; ++i) {
            if (i > 0 && i % XX_NUMWRAP == 0)
               printf("\n\t");
            XX_PRINT_RNGVAL(XX_RANDOM());
        }
        printf("\n");

        printf("  Again:");
        XX_ADVANCE(-XX_NUMVALUES);
        for (int i = 0; i < XX_NUMVALUES; ++i) {
            if (i > 0 && i % XX_NUMWRAP == 0)
               printf("\n\t");
            XX_PRINT_RNGVAL(XX_RANDOM());
        }
        printf("\n");
        

        /* Toss some coins */
        printf("  Coins: ");
        for (int i = 0; i < 65; ++i)
            printf("%c", XX_BOUNDEDRAND(2) ? 'H' : 'T');
        printf("\n");
        
        /* Roll some dice */
        printf("  Rolls:");
        for (int i = 0; i < 33; ++i)
            printf(" %d", (int) XX_BOUNDEDRAND(6)+1);
        printf("\n");
        
        /* Deal some cards */
        enum { SUITS = 4, NUMBERS = 13, CARDS = 52 };
        char cards[CARDS];
        
        for (int i = 0; i < CARDS; ++i)
           cards[i] = i;
        
        for (int i = CARDS; i > 1; --i) {
           int chosen = XX_BOUNDEDRAND(i);
           char card     = cards[chosen];
           cards[chosen] = cards[i-1];
           cards[i-1]  = card;
        }
        
        printf("  Cards:");
        static const char number[] = {'A', '2', '3', '4', '5', '6', '7',
                                      '8', '9', 'T', 'J', 'Q', 'K'};
        static const char suit[] = {'h', 'c', 'd', 's'};
        for (int i = 0; i < CARDS; ++i) {
           printf(" %c%c", number[cards[i] / SUITS], suit[cards[i] % SUITS]);
           if ((i+1) % 22 == 0)
               printf("\n\t");
        }
        printf("\n");
        
        printf("\n");
    }

    return 0;
}
