set terminal postscript enhanced eps color

set output 'com.eps'

set size 0.5, 0.5

set xlabel "time [x 10 sec.]"
set ylabel "position along x-axis [{/Symbol m}m]"

set title "Center of Mass vs. time / linear gradient"

plot './log' u ($1):2 w l lw 3 title ""