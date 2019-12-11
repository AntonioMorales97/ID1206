set terminal png
set output "pthreads_vs_green_threads.png"

set title "Pthreads vs Green threads"

set key left top

set ylabel "#Time (ms)"
set xlabel "#Loops"

plot "greens.dat" u 2:1 w linespoints linecolor 2 title "Green threads", \
      "pthreads.dat" u 2:1 w linespoints linecolor 1 title "Pthreads"
