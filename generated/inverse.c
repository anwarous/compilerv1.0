#include <stdio.h>

int main(void) {
    int n = 0;
    int inverse = 0;
    int reste = 0;

    printf("Entrez un nombre : ");
    scanf("%d", &n);
    inverse = 0;
    while ((n > 0)) {
        reste = (n % 10);
        inverse = ((inverse * 10) + reste);
        n = (n / 10);
    }
    printf("Sortie : ");
    printf("%d", inverse);
    printf("\n");
    return 0;
}
