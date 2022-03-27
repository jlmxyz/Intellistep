source testconf.sh
#set -x
openSerial

#get firmware information
sendSerial "<M115>"
readSerial

#get microstepping divisor
sendSerial "<M350>"
readSerial

#enable motor
sendSerial "<M17>"
readSerial

#get angle information
sendSerial "<M93>"
readSerial
if [ ${res} != 1.80 ]; then exit 1; fi

#direct stepping move 
#sendSerial "<G6 D0 R1000 S1000>"

#move same distance in opposite direction
#sendSerial "<G6 D1 R1000 S1000>"

