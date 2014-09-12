#include <stdio.h>
#include <stdlib.h>

static int
binary_search(int *array, int data, int length) {
    
    int low = 0;
    int high = length ;
    int mid = 0;
        
    while (high - low > 1) {
        printf("bsearch: low %d mid %d high %d arr %d\n", low, mid, high, array[mid]);
        mid = low + (high - low)/2;
        if (data == array[mid]) {
            return mid;
        } else
        if (data < array[mid]) {
            high = mid - 1;
        } else {
           low = mid;  
        }

    }

    printf("END: bsearch: low %d mid %d high %d arr %d data %d\n", low, mid, high, array[mid], data);
    if (data > array[mid])
        return ~(mid + 1);
    else
        return ~mid;
}

int
main(){

    int ar[] = { 3, 4, 8, 14, 15, 19, 23, 25, 31, 39, 43, 49, 52, 55 };
    int ar1[] = { -1 };
    int ret, i;

    for (i = 0; i<sizeof(ar1)/sizeof(ar1[0]); i++) {
        printf("[%d] %d\n", i, ar1[i]);
    }
    ret = binary_search(ar1, -2, sizeof(ar1)/sizeof(int));
    if (ret >= 0) 
        printf("binary search found ret = %d\n", ret);
    else 
        printf("not found insert at %d\n", ~ret);
    return 0;
}
    

