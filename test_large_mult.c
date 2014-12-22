#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct bignum {
    int len;
    char num[0];
};

typedef struct bignum bignum_t;

#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf(...)
#endif


bignum_t*
bignum_alloc(int len) {
    bignum_t *num = malloc(sizeof(bignum_t) + len);
    memset(num, 0, sizeof(bignum_t) + len);
    num->len = len;
    return num;
}

void
bignum_free(bignum_t *num) {
    free(num);
}

void
bignum_print(bignum_t *num) {
    int i;
    for (i = num->len; i>=0; i--) {
        printf("%d", num->num[i]);
    }
    printf("\n");
}

bignum_t*
bignum_mult(bignum_t *n1, bignum_t *n2) {
    
    bignum_t *res = bignum_alloc(n1->len + n2->len + 1);
    int i, j, k, l_res;
    int carry = 0;

    for (i = 0, k = 0; i < n2->len; i++, k++) {
        carry = 0;
        for (j = 0; j < n1->len; j++) {
            l_res =  n2->num[i]*n1->num[j] + carry;
            debug_printf("prev res %d l_res %d carry %d\n", res->num[j+k], l_res, carry);
            res->num[k+j] = res->num[k+j] + l_res;
            carry = res->num[k+j]/10;
            res->num[k+j] %= 10;
            debug_printf("res %d carry %d\n", res->num[j+k], carry);
        }
        res->num[k+j] += carry;
    }
    return res;
}

int
bignum_set(bignum_t *bignum, int num) {
    int i = 0;
    while (num && i < bignum->len) {
        bignum->num[i++] = num%10;
        num /= 10;
    }
    if (num)
        return -1;
    while(i < bignum->len) {
        bignum->num[i++] = 0;
    }
    return 0;
}

int
main() {

    bignum_t *num1, *num2, *num3;
    int i;


    num1 = bignum_alloc(4);
    num2 = bignum_alloc(4);

    /* Example */
    bignum_set(num1, 9999); 
    bignum_set(num2, 8999); 

    bignum_print(num1);
    bignum_print(num2);

    num3 = bignum_mult(num1, num2);
    bignum_print(num3);
    bignum_free(num3);

    /* Calculate 10! */
    bignum_set(num2, 1);
    for (i = 2; i<= 100; i++) {
        bignum_set(num1, i);
        num3 = bignum_mult(num1, num2);
        free(num2);
        num2 = num3;
        bignum_print(num2);
   }

    /* Sum of the digits of 10! */
    int sum_digits = 0;
    for (i = 0; i<num2->len; i++) {
        sum_digits += num2->num[i];
    }

    printf("sum digits = %d\n", sum_digits);

    return 0;
}






