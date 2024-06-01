#
# Precision Recall (Thu Apr 25 11:58:26 2024)
#

set title "Precision Recall"
set key bottom right
set autoscale
set grid
set xlabel "Recall"
set ylabel "Precision"
set tics scale 2
set terminal png small size 1000,800
set output 'network-precision-recall.png'
plot 	"network-precision-recall.tab" using 1:2 title "" with linespoints 
