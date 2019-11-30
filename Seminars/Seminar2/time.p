
set terminal png
set output "best_fit_vs_merge_no_detach_vs_worst_fit.png"

set title "Time Comparison Merge, Best Fit, and Worst Fit"

set key left center

set xlabel "#Number of allocs"
set ylabel "#Time(ms)"


plot  "time_worst_fit.dat" u 1:2 w linespoints title "worst fit", \
      "time_best_fit.dat" u 1:2 w linespoints title "best fit", \
      "time_merge_no_detach.dat" u 1:2 w linespoints title "without detach"
