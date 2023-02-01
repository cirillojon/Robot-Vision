#include <math.h>
#include <stdio.h> /*  Marr-Hildreth.c  (or marrh.c) */
#define PICSIZE 256
#define MAXMASK 100

int pic[PICSIZE][PICSIZE];
double maskx[MAXMASK][MAXMASK];
double masky[MAXMASK][MAXMASK];
double outpicx[PICSIZE][PICSIZE];
double outpicy[PICSIZE][PICSIZE];
double ival[PICSIZE][PICSIZE];
double peaksArr[PICSIZE][PICSIZE];
double edges[PICSIZE][PICSIZE];

//To compile: gcc canny.c -lm
//To run: ./a.out garb34.pgm

int main(argc, argv) int argc;
char **argv;
{
  int i, j, k, l, p, q, s, t, mr, centx, centy, control;
  double maskval, sum1, sum2, sig, maxival, dir, low, high;
  FILE *fo1, *fo2, *fo3, *fp1, *fopen();

  fp1 = fopen("garb34.pgm", "rb");
  // Magnitude
  fo1 = fopen("CannyMag.pgm", "wb");
  // peaks
  fo2 = fopen("CannyPeak.pgm", "wb");
  // Final Result
  fo3 = fopen("CannyFinal.pgm", "wb");

  fprintf(fo1, "P5\n256 256\n255\n");
  fprintf(fo2, "P5\n256 256\n255\n");
  fprintf(fo3, "P5\n256 256\n255\n");

  sig = 1;

  mr = (int)(sig * 3);
  centx = (MAXMASK / 2);
  centy = (MAXMASK / 2);

  // Remove Header

  char arr[20];

  for (i = 0; i <= 3; i++) {
    fscanf(fp1, "%s", arr);
  }

  // Part1

  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      pic[i][j] = getc(fp1);
    }
  }

  for (p = -mr; p <= mr; p++) {
    for (q = -mr; q <= mr; q++) {
      maskval = ((-q) * (exp(-1 * (((p * p) + (q * q)) / (2 * (sig * sig))))));
      (maskx[p + centy][q + centx]) = maskval;
      maskval = ((-p) * (exp(-1 * (((p * p) + (q * q)) / (2 * (sig * sig))))));
      (masky[p + centy][q + centx]) = maskval;
    }
  }

  for (i = mr; i <= 255 - mr; i++) {
    for (j = mr; j <= 255 - mr; j++) {
      sum1 = 0;
      sum2 = 0;
      for (p = -mr; p <= mr; p++) {
        for (q = -mr; q <= mr; q++) {
          sum1 += pic[i + p][j + q] * maskx[p + centy][q + centx];
          sum2 += pic[i + p][j + q] * masky[p + centy][q + centx];
        }
      }
      outpicx[i][j] = sum1;
      outpicy[i][j] = sum2;
    }
  }

  // From sobel code

  maxival = 0;
  for (i = mr; i < 256 - mr; i++) {
    for (j = mr; j < 256 - mr; j++) {
      ival[i][j] = sqrt((double)((outpicx[i][j] * outpicx[i][j]) +
                                 (outpicy[i][j] * outpicy[i][j])));
      if (ival[i][j] > maxival)
        maxival = ival[i][j];
    }
  }

  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      ival[i][j] = (ival[i][j] / maxival) * 255;
      fprintf(fo1, "%c", (char)((int)(ival[i][j])));
    }
  }

  // Part 2

  for (i = mr; i < 256 - mr; i++) {
    for (j = mr; j < 256 - mr; j++) {
      if ((outpicx[i][j]) == 0.0) {
        outpicx[i][j] = 0.00001;
      }
      dir = outpicy[i][j] / outpicx[i][j];
      if ((dir <= 0.4142) && (dir > -0.4142)) {
        if ((ival[i][j] > ival[i][j - 1]) && (ival[i][j] > ival[i][j + 1])) {
          peaksArr[i][j] = 255;
        }
      } else if ((dir <= 2.4142) && (dir > 0.4142)) {
        if ((ival[i][j] > ival[i - 1][j - 1]) &&
            (ival[i][j] > ival[i + 1][j + 1])) {
          peaksArr[i][j] = 255;
        }
      } else if ((dir <= -0.4142) && (dir > -2.4142)) {
        if ((ival[i][j] > ival[i + 1][j - 1]) &&
            (ival[i][j] > ival[i - 1][j + 1])) {
          peaksArr[i][j] = 255;
        }
      } else {
        if ((ival[i][j] > ival[i - 1][j]) && (ival[i][j] > ival[i + 1][j])) {
          peaksArr[i][j] = 255;
        }
      }
    }
  }

  for (i = 0; i < PICSIZE; i++) {
    for (j = 0; j < PICSIZE; j++) {
      if (peaksArr[i][j] == 255)
        fprintf(fo2, "%c", (char)((int)(255)));
      else
        fprintf(fo2, "%c", (char)((int)(0)));
    }
  }

  // part 3

  double Percent = 0.05;
  int Histogram[PICSIZE] = {0};
  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      (Histogram[(int)ival[i][j]])++;
    }
  }
  double CutOff = Percent * PICSIZE * PICSIZE;
  double AreaOfTops = 0;
  for (i = 255; i > 0; i--) {

    AreaOfTops += Histogram[i];

    if (AreaOfTops > CutOff) {
      break;
    }
  }

  high = i;
  low = 0.35 * high;

  // part 3

  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      if (peaksArr[i][j] == 255) {
        if (ival[i][j] > high) {
          peaksArr[i][j] = 0;
          edges[i][j] = 255;
        } else if (ival[i][j] < low) {
          peaksArr[i][j] = 0;
          edges[i][j] = 0;
        }
      }
    }
  }
  control = 1;
  while (control == 1) {
    control = 0;
    for (i = 0; i < 256; i++) {
      for (j = 0; j < 256; j++) {
        if (peaksArr[i][j] == 255) {
          for (k = -1; k <= 1; k++) {
            for (l = -1; l <= 1; l++) {
              if (edges[i + k][j + l] == 255) {
                peaksArr[i][j] = 0;
                edges[i][j] = 255;
                control = 1;
              }
            }
          }
        }
      }
    }
  }

  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      fprintf(fo3, "%c", (char)((int)(edges[i][j])));
    }
  }

  fclose(fp1);
  fclose(fo1);
  fclose(fo2);
  fclose(fo3);
  return 0;
}