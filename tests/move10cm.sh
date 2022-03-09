source testconf.sh

openSerial

#enable motor
sendSerial <M17>
res=$(readSerial)

#get angle information
sendSerial <M93>
res=$(readSerial)

#get firmware information
sendSerial <M115>
res=$(readSerial)

#get microstepping divisor
sendSerial <M350>
res=$(readSerial)

#direct stepping move 
sendSerial <G6 D0 R1000 S1000>

#move same distance in opposite direction
sendSerial <G6 D1 R1000 S1000>

