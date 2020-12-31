#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>

static void burg_algorithm(const double* data, uint32_t num_samples, double* coef, uint32_t coef_order)
{
  uint32_t i, k;
  double *a_vec;
  double *f_vec;
  double *b_vec;
  double *parcor_coef;
  double Dk, mu;
  double tmp1, tmp2;

  /* ベクトル領域割り当て */
  a_vec = malloc(sizeof(double) * (coef_order + 1));
  parcor_coef = malloc(sizeof(double) * (coef_order + 1));
  f_vec = malloc(sizeof(double) * num_samples);
  b_vec = malloc(sizeof(double) * num_samples);

  /* 各ベクトル初期化 */
  for (k = 0; k < coef_order + 1; k++) {
    a_vec[k] = 0.0f;
  }
  a_vec[0] = 1.0f;
  memcpy(f_vec, data, sizeof(double) * num_samples);
  memcpy(b_vec, data, sizeof(double) * num_samples);

  /* Dkの初期化 */
  Dk = 0.0f;
  for (i = 0; i < num_samples; i++) {
    Dk += 2.0f * f_vec[i] * f_vec[i];
  }
  Dk -= f_vec[0] * f_vec[0] + b_vec[num_samples - 1] * b_vec[num_samples - 1];

  /* Burg 再帰アルゴリズム */
  for (k = 0; k < coef_order; k++) {
    /* 反射（PARCOR）係数の計算 */
    mu = 0.0f;
    for (i = 0; i < num_samples - k - 1; i++) {
      mu += f_vec[i + k + 1] * b_vec[i];
    }
    mu *= -2.0f / Dk;
    parcor_coef[k] = -mu;
    assert(fabs(parcor_coef[k]) < 1.0f);

    /* a_vecの更新 */
    for (i = 0; i <= (k + 1) / 2; i++) {
      tmp1 = a_vec[i]; tmp2 = a_vec[k + 1 - i];
      a_vec[i]         = tmp1 + mu * tmp2;
      a_vec[k + 1 - i] = mu * tmp1 + tmp2;
    }

    /* f_vec, b_vecの更新 */
    for (i = 0; i < num_samples - k - 1; i++) {
      tmp1 = f_vec[i + k + 1]; tmp2 = b_vec[i];
      f_vec[i + k + 1] = tmp1 + mu * tmp2;
      b_vec[i]         = mu * tmp1 + tmp2;
    }

    /* Dkの更新 */
    Dk = (1.0f - mu * mu) * Dk - f_vec[k + 1] * f_vec[k + 1] - b_vec[num_samples - k - 2] * b_vec[num_samples - k - 2];
  }

  /* 係数コピー */
  memcpy(coef, a_vec, sizeof(double) * (coef_order + 1));

  free(b_vec);
  free(f_vec);
  free(a_vec);
  free(parcor_coef);
}

int main(void)
{
  const uint32_t num_samples  = 32 * 8192;
  const uint32_t coef_order   = 256;
  uint32_t i, j;
  double* data;
  double* predict;
  double* coef;
  double err;

  data    = malloc(sizeof(double) * num_samples);
  predict = malloc(sizeof(double) * num_samples);
  coef    = malloc(sizeof(double) * (coef_order + 1));

  /* データ作成 */
  for (i = 0; i < num_samples; i++) {
    data[i] = sin(i * 0.01f) + cos(4.0f * sin(i * 0.05f));
  }

  /* Burgアルゴリズム */
  burg_algorithm(data, num_samples, coef, coef_order);

  /* 予測 */
  err = 0.0f;
  for (i = coef_order; i < num_samples; i++) {
    predict[i] = 0.0f;
    for (j = 1; j <= coef_order; j++) {
      predict[i] -= coef[j] * data[i - j];
    }
    err += pow(data[i] - predict[i], 2);
  }
  printf("error:%e \n", sqrt(err / (num_samples - coef_order)));

  free(data);
  free(predict);
  free(coef);

  return 0;
}
