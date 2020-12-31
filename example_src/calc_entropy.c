#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char **argv)
{
  FILE*     fp;
  int       byte;
  double    entropy;                      
  uint32_t  count_sum;                    /* 頻度の総和                     */
  uint32_t  byte_count[0x100] = { 0, };   /* 各バイトの出現数カウント       */

  /* 引数が少ない場合は使用方法を表示 */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s filename \n", argv[0]);
    return 1;
  }

  /* バイナリモードでファイルオープン */
  fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    fprintf(stderr, "Cannnot open %s. \n", argv[1]);
    return 1;
  }

  /* 各バイトの出現頻度をカウント */
  count_sum = 0;
  while ((byte = fgetc(fp)) != EOF) {
    byte_count[byte & 0xFF]++;
    count_sum++;
  }

  /* ファイルクローズ */
  fclose(fp);

  /* エントロピー計算 */
  entropy = 0.0f;
  for (byte = 0; byte < 0x100; byte++) {
    /* 頻度0のパターンについては 0 * log2(0) = 0 だから加算しない */
    if (byte_count[byte] > 0) {
      /* 経験確率の計算 */
      double prob = (double)byte_count[byte] / count_sum;
      entropy -= prob * log2(prob);
    }
  }

  /* 結果を表示 */
  printf("entropy: %8.6f[byte] \n", entropy);

  /* 正常終了 */
  return 0;
}
