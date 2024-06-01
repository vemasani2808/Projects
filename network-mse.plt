#
# MSE (Thu Apr 25 11:58:27 2024)
#

set title "MSE"
set key bottom right
set autoscale
set grid
set xlabel "Time"
set ylabel "MSE"
set tics scale 2
set terminal png small size 1000,800
set output 'network-mse.png'
plot 	"network-mse.tab" using 1:2 title "" with linespoints 
