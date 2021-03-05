/*daa_11_matrix.c*/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#define INFINITY 999999

int main(void)
{
  int i, j, k, l, n = 6, q;
  int p[] = {4, 3, 2, 1, 5, 6, 7};
  int m[10][10];
  int s[10][10];

  for(i = 1; i <= n; i++)
  {
    m[i][i] = 0;
  }

  for(l = 2; l <= n; l++)
  {
    for(i = 1; i <= n - l + 1; i++)
    {
      j = i + l - 1;
      m[i][j] = INFINITY;

      for(k = i; k <= j - 1; k++)
      {
        q = m[i][k] + m[k + 1][j] + p[i - 1]* p[k] * p[j];

        if(q < m[i][j])
        {
          m[i][j] = q;
          s[i][j]  = k;
        }
      }
    }
  }

  for(l = n; l >= 1; l--)
  {
    printf("\n");

    for(i = 1; i <= n - l + 1; i++)
    {
      printf("    %d    ", m[i][i + l - 1]);
    }
  }

  for(l = n; l >= 2; l--)
  {
    printf("\n");

    for(i = 1; i <= n - l + 1; i++)
    {
      printf("    %d    ", s[i][i + l - 1]);
    }
  }
 
  return 0;
}
