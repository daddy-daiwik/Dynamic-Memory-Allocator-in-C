#include <stdio.h>
#include <unistd.h>
#include "my_alloc.h"
#include <stddef.h>

int main() {
    int *arr = my_malloc(sizeof(int) * 5);
    char *string = my_malloc(sizeof(char) * 45);
    string = "Team 5: Shake, Roy, Deer, Teddy, Sowju\0";
    for(int i = 0; i < 5; i++) {
        arr[i] = i * 10;
        printf("%d\n", arr[i]);
    }
    printf("\nString: %s\n", string);
    my_free(arr);
    my_free(string);
    return 0;
}