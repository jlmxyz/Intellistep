source testconf.sh

mmPerTurn=0.8
distanceToTravel_mm=50
#speed in mm/s
speed=2

#uncomment following line if you want to activate trace mode 
#set -x
openSerial

#get firmware information
sendSerial "<M115>"
readSerial

#get firmware configuration
sendSerial "<M503>"
readSerial

#set microstepping divisor
sendSerial "<M350 S1>"
readSerial 
#get microstepping divisor
sendSerial "<M350>"
readSerial
microstepping=${res}

#enable motor
sendSerial "<M17>"
readSerial

#get angle information
sendSerial "<M93>"
readSerial
if [ ${res} != 1.80 ]; then exit 1; fi
set -x
stepPerTurn=$(echo "(360*${microstepping})/${res}" | bc)
stepPermm=$(echo "${stepPerTurn}/${mmPerTurn}" | bc)
nbOfSteps=$(echo "${stepPermm}*${distanceToTravel_mm}" | bc)
duration_s=$(echo "${distanceToTravel_mm}/${speed}" | bc)
rate=$(echo "${nbOfSteps}/${duration_s}" | bc)

cat <<EOF 
paramters :
- mm per turn : ${mmPerTurn}
- distance to travel : ${distanceToTravel_mm}
- speed : ${speed} mm/s

motor configuration:
- microstepping : ${microstepping}

computed :
- step per turn : ${stepPerTurn}
- step per mm : ${stepPermm}
- nb of steps : ${nbOfSteps}
- rate : ${rate}
EOF


#direct stepping move 
sendSerial "<G6 D0 R${rate} S${nbOfSteps}>"
readSerial
echo "verify that ${distanceToTravel_mm}mm was traveled in one direction (y/n)"
read Answer
if [ "${Answer}" != "y" ]; then exit 1; fi

#move same distance in opposite direction
sendSerial "<G6 D1 R${rate} S${nbOfSteps}>"
readSerial
echo "verify that ${distanceToTravel_mm}mm was traveled in opposite direction (y/n)"
read Answer
if [ "${Answer}" != "y" ]; then exit 1; fi
