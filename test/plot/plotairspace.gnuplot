set size ratio -1
set key outside right

set style line 1 lt 2 lc rgb "black" lw 2
set style line 2 lt 4 lc rgb "purple" lw 1
set style line 3 lt 4 lc rgb "orange" lw 1
set style line 4 lt 4 lc rgb "blue" lw 2
set style line 5 lt 4 lc rgb "grey" lw 2
set style line 6 lt 4 lc rgb "green" lw 1
set style line 7 lt 4 lc rgb "cyan" lw 1
set style line 8 lt 2 lc rgb "red" lw 2


plot \
     'results/res-bb-in.txt' using 1:2 with lines ls 3 title "all airspace", \
     'results/res-bb-range.txt' using 1:2 with filledcurve ls 5 title "in range", \
     'results/res-bb-inside.txt' using 1:2 with filledcurve ls 8 title "inside airspace", \
     'results/res-sample.txt' using 2:3 with lines ls 1 title "sample"
pause -1

plot \
     'results/res-bb-range.txt' using 1:2 with filledcurve ls 5 title "in range", \
     'results/res-bb-sortednearest.txt' using 1:2 with filledcurve ls 2 title "sorted nearest", \
     'results/res-bb-range.txt' using 1:2 with lines ls 4 title "", \
     'results/res-bb-sortedsoonest.txt' using 1:2 with lines ls 8 title "sorted soonest", \
     'results/res-bb-closest.txt' using 1:2 with lines ls 3 title "closest"
pause -1

plot \
     'results/res-bb-in.txt' using 1:2 with lines ls 5 title "all airspace", \
     'results/res-bb-intersected.txt' using 1:2 with filledcurve ls 3 title "airspace", \
     'results/res-bb-intersected.txt' using 1:2 with lines ls 5 title "airspace", \
     'results/res-bb-intersects.txt' using 1:2 with linespoints ls 4 title "intersects", \
     'results/res-bb-intercepts.txt' using 1:2 with points ls 1 title "intercepts"

pause -1

set title "Airspace Warning Manager"
plot \
     'results/res-bb-in.txt' using 1:2 with lines ls 5 title "all airspace", \
     'results/res-as-warnings-inside.txt' using 1:2 with lines ls 8 title "inside", \
     'results/res-as-warnings-glide.txt' using 1:2 with lines ls 3 title "glide", \
     'results/res-as-warnings-filter.txt' using 1:2 with lines ls 2 title "filter", \
     'results/res-as-warnings-task.txt' using 1:2 with lines ls 4 title "Task", \
     'results/res-sample.txt' using 2:3 with lines ls 1 title "sample"
pause -1
