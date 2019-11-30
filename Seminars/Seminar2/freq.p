
set terminal png
set output "best_worst_fit_comparison.png"

set title "Best Fit vs Worst Fit"

set key left top

set xlabel "#Number of allocs"
set ylabel "#Length of flist"


plot  "worst_fit_flist.dat" u 1:2 w linespoints title "worst fit", \
      "merge.dat" u 1:2 w linespoints title "merge without detach", \
      "best_fit_flist.dat" u 1:2 w linespoints title "best fit"
