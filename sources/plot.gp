set terminal wxt persist
set xlabel 'Space (x)'
set ylabel 'Temperature (u(t))'
#set offsets graph 0.01, 0.01, 0.01, 0.01
set grid xtics ytics
set style fill transparent solid 0.333
#plot '5000.dat' using 1:2 title 'u(x) 5000' with lines, \
#  '500.dat' using 1:2 title 'u(x) 500' with lines
filename(n) = sprintf("%d.dat", n)
plot for [i = 10:990:100] filename(i) using 1:2 title 'u(x) ('.i.')' with lines
