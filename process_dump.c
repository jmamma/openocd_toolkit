#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
      printf("Filename required\n");
      return 1;
    }

    FILE *fp;
    fp = fopen(argv[1], "r");
    uint32_t word;
    uint16_t count;	

    while (fread(&word, 4, 1, fp)) {
      uint8_t *bytes = (uint8_t *) &word;
      
      uint32_t word_s;
      
      //Reverse Endianness.
      ((uint8_t *)&word_s)[0] = bytes[3];
      ((uint8_t *)&word_s)[1] = bytes[2];
      ((uint8_t *)&word_s)[2] = bytes[1];
      ((uint8_t *)&word_s)[3] = bytes[0];

      /* Fractional Representation
      $7FFFFF: 1 - 2^-23 = 0.9999998  (Most positive)
      $800000; 2^23 = -1.0            (Most negative)
      */
      float out;
      float neg = 1;
      
      //Negative
      if (word_s >= (uint32_t) (1 << 23)) { 
        word_s = 0x1000000 - word_s;  //(2 ^ 23) - (word_s - (2 ^ 23))
        neg = -1;  
        out = (float) word_s / (float) 0x800000 * -1;
      }
      //Positive
      else {
        out = (float) word_s / (float) 0x7FFFFF;
      }     
 
      //printf("%d %X %.24f\n", count, word_s, out);

      printf("%.24f\n", out);
      count += 4;
    }

    return 0;
}

