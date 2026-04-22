#include <stdio.h>
#include <unistd.h>
#include "my_alloc.h"
#include <stddef.h>
#include <string.h>
int main() {
    
    //Character array
    char *string = my_malloc(sizeof(char) * 40);
    strcpy(string, "Aunty, Daddy, Madam, Kid, Akka");
    printf("String: %s\n", string);
    // print_stats();
    my_free(string);
    // print_stats();
    //Integer array
    int *arr = my_calloc(5, sizeof(int));
    for(int i = 0; i < 5; i++) {
        arr[i] = i * 10;
        printf("%d\n", arr[i]);
    }
    // print_stats();
    my_free(arr);
    // print_stats();
    print_heap();
    return 0;
}
