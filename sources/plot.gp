set terminal wxt persist
set xlabel 'Space (x)'
set ylabel 'Temperature (u(x))'
#set offsets graph 0.01, 0.01, 0.01, 0.01
set grid xtics ytics
set style fill transparent solid 0.333
filename(x) = sprintf("%d.dat", x)
set key autotitle columnhead
plot for [i = 0:1000:100] filename(i) using 1:2 with lines
