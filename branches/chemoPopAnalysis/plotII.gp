set terminal postscript enhanced eps color

set output 'com_no.eps'

set size 0.5, 0.5

set xlabel "time [x 10 sec.]"
set ylabel "position along x-axis [{/Symbol m}m]"

set title "Center of Mass vs. time / no gradient"

plot './log2' u ($1):2 w l lw 3 title ""