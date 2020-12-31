#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdint.h>

/* （標本）自己相関の計算 */
static int32_t 
calc_auto_correlation(double *auto_corr, const double *data, uint32_t num_sample, uint32_t max_order);

/* Levinson-Durbin再帰計算 */
static int32_t 
levinson_durbin_recursion(double *lpc_coef, const double *auto_corr, uint32_t max_order);

int main(void) 
{
  int32_t num_sample = 300; /* サンプル数 */
  int32_t max_delay  = 10;  /* LPC係数の数 */

  int32_t smpl, i;
  double *data     = (double *)malloc(sizeof(double) * num_sample);
  double *predict  = (double *)malloc(sizeof(double) * num_sample);
  double *auto_cor = (double *)malloc(sizeof(double) * (max_delay + 1));
  double *coff     = (double *)malloc(sizeof(double) * (max_delay + 1));
  double error;

  /* 波形の生成 */
  for (smpl = 0; smpl < num_sample; smpl++) {
    data[smpl] = sin(smpl * 0.01) + 0.5 * cos(4.0f * sin(smpl * 0.05));
  }

  /* 自己相関・Levinson-Durbin再帰計算 */
  calc_auto_correlation(auto_cor, data, num_sample, max_delay + 1);
  levinson_durbin_recursion(coff, auto_cor, max_delay);

  /* 予測テスト */
  for (smpl = 0; smpl < num_sample; smpl++) {
    if (smpl < max_delay) {
      /* 最初のmax_delayステップ分は元信号を単純コピー */
      predict[smpl] = data[smpl];
    } else { 
      /* 以降は予測 */
      predict[smpl] = 0.0f;
      for (i = 1; i <= max_delay; i++) {
        predict[smpl] -= (coff[i] * data[smpl-i]);
      }
    }
  }

  /* 誤差計算・結果表示 */
  error = 0.0f;
  for (smpl = 0; smpl < num_sample; smpl++) {
    error += pow(predict[smpl] - data[smpl], 2);
    printf("No:%d Data: %f Predict: %f \n", smpl, data[smpl], predict[smpl]);
  }
  printf("Error : %f \n", sqrt(error / num_sample));

  free(data); free(predict);
  free(auto_cor); free(coff);

  return 0;
}

static int32_t levinson_durbin_recursion(double *lpc_coef, const double *auto_corr, uint32_t max_order)
{
  int32_t k, i;
  double lambda;
  double *u_vec, *v_vec, *a_vec, *e_vec;

  if (lpc_coef == NULL || auto_corr == NULL) {
    fprintf(stderr, "Data or result pointer point to NULL. \n");
    return -1;
  }

  /* 
   * 0次自己相関（信号の二乗和）が0に近い場合、入力信号は無音と判定
   * => 予測誤差, LPC係数は全て0として無音出力システムを予測.
   */
  if (fabs(auto_corr[0]) < FLT_EPSILON) {
    for (i = 0; i < max_order + 1; i++) {
      lpc_coef[i] = 0.0f;
    }
    return 0;
  }

  /* 初期化 */
  a_vec = (double *)malloc(sizeof(double) * (max_order + 2)); /* a_0, a_k+1を含めるとmax_order+2 */
  e_vec = (double *)malloc(sizeof(double) * (max_order + 2)); /* e_0, e_k+1を含めるとmax_order+2 */
  u_vec = (double *)malloc(sizeof(double) * (max_order + 2));
  v_vec = (double *)malloc(sizeof(double) * (max_order + 2));
  for (i = 0; i < max_order + 2; i++) {
    u_vec[i] = v_vec[i] = a_vec[i] = 0.0f;
  }

  /* 最初のステップの係数をセット */
  a_vec[0] = 1.0f;
  e_vec[0] = auto_corr[0];
  a_vec[1] = - auto_corr[1] / auto_corr[0];
  e_vec[1] = auto_corr[0] + auto_corr[1] * a_vec[1];
  u_vec[0] = 1.0f; u_vec[1] = 0.0f; 
  v_vec[0] = 0.0f; v_vec[1] = 1.0f; 

  /* 再帰処理 */
  for (k = 1; k < max_order; k++) {
    lambda = 0.0f;
    for (i = 0; i < k + 1; i++) {
      lambda += a_vec[i] * auto_corr[k + 1 - i];
    }
    lambda /= (-e_vec[k]);
    e_vec[k + 1] = (1 - lambda * lambda) * e_vec[k];

    /* u_vec, v_vecの更新 */
    for (i = 0; i < k; i++) {
      u_vec[i + 1] = v_vec[k - i] = a_vec[i + 1];
    }
    u_vec[0] = 1.0f; u_vec[k + 1] = 0.0f;
    v_vec[0] = 0.0f; v_vec[k + 1] = 1.0f;

    /* resultの更新 */
    for (i = 0; i < k+2; i++) {
      a_vec[i] = u_vec[i] + lambda * v_vec[i];
    }
  }

  /* 結果の取得 */
  memcpy(lpc_coef, a_vec, sizeof(double) * (max_order + 1));

  free(u_vec); free(v_vec);
  free(a_vec); free(e_vec); 

  return 0;
}

static int32_t calc_auto_correlation(double *auto_corr, const double *data, uint32_t num_sample, uint32_t max_order)
{
  int32_t smpl, i;

  if (max_order > num_sample) {
    fprintf(stderr, "Max order(%d) is larger than number of samples(%d). \n", max_order, num_sample);
    return -1;
  }

  if (auto_corr == NULL || data == NULL) {
    fprintf(stderr, "Data or result pointer point to NULL. \n");
    return -2;
  }

  /* （標本）自己相関の計算 */
  for (i = 0; i < max_order; i++) {
    auto_corr[i] = 0.0f;
    for (smpl = i; smpl < num_sample; smpl++) {
      auto_corr[i] += data[smpl] * data[smpl - i];
    }
  }

  return 0;
}
