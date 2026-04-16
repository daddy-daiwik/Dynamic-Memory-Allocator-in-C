#include <stdio.h>
#include <unistd.h>
#include "my_alloc.h"
#include <stddef.h>
#include <string.h>
int main() {
    
    char *string = my_malloc(sizeof(char) * 40);
    strcpy(string, "Team 5: Aunty, Daddy, Madam, Kid, Akka, Anna");
    printf("String: %s\n", string);
    my_free(string);
    int *arr = my_malloc(sizeof(int) * 5);
    for(int i = 0; i < 5; i++) {
        arr[i] = i * 10;
        printf("%d\n", arr[i]);
    }
    
    my_free(arr);
    return 0;
}