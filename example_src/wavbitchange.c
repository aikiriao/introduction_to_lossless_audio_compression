#include "wav.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  struct WAVFile* wavfile;

  /* 引数の数が誤っている場合は使用方法を表示 */
  if (argc != 4) {
    fprintf(stderr, "Usage: program bits infile outfile \n");
    return 1;
  }

  /* wavファイルの読み込み */
  if ((wavfile = WAV_CreateFromFile(argv[2])) == NULL) {
    fprintf(stderr, "Failed to get wav data. \n");
    return 1;
  }

  /* 変換後のbitの取得 */
  wavfile->format.bits_per_sample = atoi(argv[1]);
  if ((wavfile->format.bits_per_sample != 8)
      && (wavfile->format.bits_per_sample != 16)
      && (wavfile->format.bits_per_sample != 24)
      && (wavfile->format.bits_per_sample != 32)) {
    fprintf(stderr, "Wavfile bps must be 8/16/24/32. \n");
    return 1;
  }

  /* bitを変更して保存 */
  WAV_WriteToFile(argv[3], wavfile);

  /* ハンドル破棄 */
  WAV_Destroy(wavfile);

  return 0;
}
