#include <stdio.h>

int main(void) {
    int n = 0;

    printf("Entrez un entier : ");
    scanf("%d", &n);
    if (((n % 2) == 0)) {
        if ((n >= 0)) {
            printf("Pair et Positif");
            printf("\n");
        } else {
            printf("Pair et Négatif");
            printf("\n");
        }
    } else {
        if ((n >= 0)) {
            printf("Impair et Positif");
            printf("\n");
        } else {
            printf("Impair et Négatif");
            printf("\n");
        }
    }
    return 0;
}
