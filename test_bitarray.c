/*
 *  A bitarray implementation.
 *  Fredrick John Berchmans 
 *  fredrickprashanth@gmail.com
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#ifndef NDEBUG
#define debug_printf(format, ...) printf("%s:%d " format , __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define debug_printf(...)
#endif

#define BIT_N        8
#define BIT_LOG      3
#define BIT_MSK      (BIT_N - 1)
#define BIT_IDX(bit) ((bit) >> BIT_LOG)
#define BIT_POS(bit) ((bit) & BIT_MSK)
#define BITMASK(bits) ((1 << (bits)) - 1)
#define BITARRAY_WORD_WIDTH(ba) (BIT_IDX(ba->nr_bits))
struct bitarray {
    int nr_bits;
    unsigned char array[0];
};


struct bitarray*
bitarray_create(int nr_bits) {
    struct bitarray *ba;
    if (nr_bits & 0x7)
        return NULL;
    ba = malloc(sizeof(struct bitarray) + (nr_bits>>BIT_LOG));
    memset(ba, 0, sizeof(struct bitarray) + (nr_bits>>BIT_LOG));

    ba->nr_bits = nr_bits;
    return ba;
}

int
bitarray_set(struct bitarray *ba, int bit) {
    if (bit > ba->nr_bits)
        return -EINVAL;
    ba->array[BIT_IDX(bit)] |= 1<<(BIT_POS(bit));
    debug_printf("ba array [ %d ] = %x\n", BIT_IDX(bit), ba->array[BIT_IDX(bit)]);
    return 0;
}

int
bitarray_get(struct bitarray *ba, int bit) {
    if (bit > ba->nr_bits)
        return -EINVAL;
    debug_printf("ba array [ %d ] = %x\n", BIT_IDX(bit), ba->array[BIT_IDX(bit)]);
    return (ba->array[BIT_IDX(bit)] & (1<<BIT_POS(bit))) ? 1 : 0;
}

void
bitarray_print(struct bitarray *ba) {
    int i;
    for (i=BITARRAY_WORD_WIDTH(ba)-1; i>=0; i--) {
        printf(" %02x ", ba->array[i]);
    }
    printf("\n");
}
    

int
bitarray_shift(struct bitarray *ba, int dir, int bits) {
/*
 * dir = 0 left shift
 * dir = 1 right shift
 */
    int i;
    int bits_shift, bitmask;
    /* shift width module the word size */
    int shift_width_words = BIT_IDX(bits) > BIT_IDX(ba->nr_bits) ? BIT_IDX(ba->nr_bits) : BIT_IDX(bits);
    int rest_shift = bits & BIT_MSK;

    debug_printf("shift_width_words: %d rest_shift: %d\n", shift_width_words, rest_shift);
    
    /* shifting more than size of the bitarray */
    if (bits >= BITARRAY_WORD_WIDTH(ba)) {
        memset(ba->array, 0 , BITARRAY_WORD_WIDTH(ba));
        return 0;
    }
        

    if (dir == 1) {
        /* right shift */

        /* x bits on the right end disappears */
        for (i=shift_width_words; i<BITARRAY_WORD_WIDTH(ba); i++) {
            bitmask = BITMASK(rest_shift);
            /* save the bits that will go to the right */
            bits_shift = ba->array[i+1] & bitmask;
            debug_printf("before array [ %d ] = %x\n", i, ba->array[i]);
            debug_printf("before array [ %d ] = %x\n", i+1, ba->array[i+1]);
            debug_printf("shift bits %x\n", bits_shift);
            /* do the shift */
            ba->array[i] >>= rest_shift;
            debug_printf("after array [ %d ] = %x\n", i, ba->array[i]);
            ba->array[i] &= bitmask;
            debug_printf("after mask array [ %d ] = %x\n", i, ba->array[i]);
            /* apply the saved bits to the right of i ie. i-1 */
            ba->array[i] |= bits_shift<<(BIT_N - rest_shift);
            debug_printf("after apply array [ %d ] = %x\n", i, ba->array[i]);
        }
        /* move right to perform the shift */
        memmove(&ba->array[0], 
                &ba->array[shift_width_words], 
                BITARRAY_WORD_WIDTH(ba) - shift_width_words);
        /* zero out the leftmost bits */
        memset(&ba->array[BITARRAY_WORD_WIDTH(ba) - 1 - shift_width_words], 0,
               shift_width_words);

    
    } else {
        /* left shift */

        /* x bits on the left end disappears and from there onwards the left shift happens */
        for (i=BITARRAY_WORD_WIDTH(ba) - shift_width_words - 1; i>=0; i--) {
            bitmask = BITMASK(rest_shift) << (BIT_N - rest_shift);
            debug_printf("before array [ %d ] = %x mask = %x\n", i, ba->array[i], bitmask);
            /* get the bits that will go to the left of i */
            bits_shift = ba->array[i-1] & bitmask;
            /* do the shift */
            ba->array[i] <<= rest_shift;
            debug_printf("after array [ %d ] = %x\n", i, ba->array[i]);
            /* apply the bits to i */
            debug_printf("before array [ %d ] = %x\n", i, ba->array[i]);
            ba->array[i] &= bitmask;
            debug_printf("after array [ %d ] = %x\n", i, ba->array[i]);
            ba->array[i] |= bits_shift>>(BIT_N - rest_shift);
            debug_printf("after array [ %d ] = %x\n", i, ba->array[i]);
        }
        
        debug_printf("dest idx %d src idx %d len %d\n",
                      BITARRAY_WORD_WIDTH(ba) - 1,
                      BITARRAY_WORD_WIDTH(ba) - shift_width_words - 1,
                      BITARRAY_WORD_WIDTH(ba) - shift_width_words);

        /* move left to perform the shift */
        memmove(&ba->array[shift_width_words], 
                &ba->array[0],
                BITARRAY_WORD_WIDTH(ba) - shift_width_words);
        /* zero out the rightmost bits */
        memset(&ba->array[0], 0, shift_width_words);

    }

    return 0;
    
}

int
main(int argc, char *argv[]) {

    struct bitarray *ba;
    int rc;
    int bit, bitset;

#define BSIZE 128

    ba = bitarray_create(BSIZE);

    for (bit = 0; bit<BSIZE; bit++) {
        bitset = 0;
        if (random() & 1) {
            if ((rc = bitarray_set(ba, bit)) < 0) {
                printf("bitarray_set of bit %d failed with err %d\n", bit, rc);
                return -1;
            }
            printf("setting bit %d\n", bit);
            bitset = 1;
        }
        if ((rc = bitarray_get(ba, bit)) < 0) {
            printf("bitarray_get of bit %d failed with err %d\n", bit, rc);
            return -1;
        }
        printf("bitarray bit %d value %d\n", bit, rc);
        if (bitset != rc ) {
            printf("bitset test failed for bit %d\n", bit);
            return -1;
        }
    }

    bitarray_print(ba);
    bitarray_shift(ba, 0, 128);
    bitarray_print(ba);

    return 0;
}
