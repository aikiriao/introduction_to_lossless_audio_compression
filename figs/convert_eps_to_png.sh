#!/bin/bash

for eps in "$@"
do
  echo $eps
  convert -density 600 -units PixelsPerInch $eps ${eps%.eps}.png
done
