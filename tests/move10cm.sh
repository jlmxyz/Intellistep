source testconf.sh

mmPerTurn=2
distanceToTravel=50
speed=1000

#set -x
openSerial

#get firmware information
sendSerial "<M115>"
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
stepPerTurn=$(echo "360/(${res}*${microstepping})" | bc)
stepPermm=$(echo "${stepPerTurn}/${mmPerTurn}" | bc)
nbOfSteps=$(echo "${stepPermm}*${distanceToTravel}" | bc)


#direct stepping move 
sendSerial "<G6 D0 R${nbOfSteps} S${speed}>"
echo "verify that ${distanceToTravel}mm was traveled in one direction (Y/N)"
read Answer
if [ "${Answer}" != "Y" ]; then exit 1; fi

#move same distance in opposite direction
sendSerial "<G6 D1 R${nbOfSteps} S${speed}>"
echo "verify that ${distanceToTravel}mm was traveled in opposite direction (Y/N)"
if [ "${Answer}" != "Y" ]; then exit 1; fi
