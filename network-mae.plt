#
# Normalized MAE (Thu Apr 25 11:58:27 2024)
#

set title "Normalized MAE"
set key bottom right
set autoscale
set grid
set xlabel "Time"
set ylabel "Normalized MAE"
set tics scale 2
set terminal png small size 1000,800
set output 'network-mae.png'
plot 	"network-mae.tab" using 1:2 title "" with linespoints 
