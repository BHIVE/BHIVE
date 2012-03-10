set terminal postscript enhanced eps color

set output 'com.eps'

set size 0.5, 0.5

set xlabel "time [x 10 sec.]"
set ylabel "position along x-axis [{/Symbol m}m]"

set title "Center of Mass vs. time / linear gradient"

set key left

plot './ergebnis' u ($1/10):2 w l lw 3 title "{/Symbol t}=2.5",\
'./ergebnis' u ($1/10):5 w l lw 3 title "{/Symbol t}=7.5",\
'./ergebnis' u ($1/10):8 w l lw 3 title "{/Symbol t}=12.5",\
'./ergebnis' u ($1/10):11 w l lw 3 title "{/Symbol t}=17.5"