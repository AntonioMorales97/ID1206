# Gnuplot script for plotting data from the .dat files
set terminal png
set output "benches.png"

set title "Page replacement policies"

set key right center

set xlabel "frames in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "random.dat" u 1:2 w linespoints title "random", \
    "optimal.dat" u 1:2 w linespoints title "optimal", \
    "lru.dat" u 1:2 w linespoints title "lru", \
    "clock.dat" u 1:2 w linespoints title "clock"
