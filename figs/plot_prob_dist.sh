#!/bin/bash

ALPHA_GAMMA_CODE_DIST_NAME="alpha_gamma_code_dist.eps"
GOLOMB_CODE_DIST_NAME="geometric_dist.eps"

gnuplot << EOF
  set terminal postscript enhanced
  f(rho,x) = ((rho) ** x) * (1 - rho)
  set xlabel 'x'
  set ylabel 'Probability'
  set xrange [1:10]
  set xtics 1
  set grid
  set output "${ALPHA_GAMMA_CODE_DIST_NAME}"
  plot 2 ** (-x) with line linewidth 2 title "Alpha code", 1 / (2 * (x ** 2)) with line linewidth 2 title "Gamma code"
  set xrange [0:10]
  set output "${GOLOMB_CODE_DIST_NAME}"
  plot f(0.2,x) with line linewidth 2 title "{/Symbol r} = 0.2", \
       f(0.4,x) with line linewidth 2 title "{/Symbol r} = 0.4", \
       f(0.6,x) with line linewidth 2 title "{/Symbol r} = 0.6", \
       f(0.8,x) with line linewidth 2 title "{/Symbol r} = 0.8"
EOF
