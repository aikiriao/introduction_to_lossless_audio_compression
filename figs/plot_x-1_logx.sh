#!/bin/bash
gnuplot << EOF
  set terminal postscript enhanced
  set xlabel 'x'
  set xrange [-1:5]
  set xtics 1
  set yrange [-3:3]
  set ytics 1
  set grid
  set output "x-1_logx.eps"
  plot x - 1 with line linewidth 3 title "x-1", \
       log(x) with line linewidth 3 title "log(x)"
EOF
