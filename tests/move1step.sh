source testconf.sh

nbOfSteps=1
rate=1
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

cat <<EOF 
paramters :
- nbOfSteps : ${nbOfSteps}

motor configuration:
- microstepping : ${microstepping}

EOF


#direct stepping move 
sendSerial "<G6 D0 R${rate} S${nbOfSteps}>"
readSerial
echo "verify that motor turned ${nbOfSteps} in one direction (y/n)"
read Answer
if [ "${Answer}" != "y" ]; then exit 1; fi

#move same distance in opposite direction
sendSerial "<G6 D1 R${rate} S${nbOfSteps}>"
readSerial
echo "verify that motor turned ${nbOfSteps} opposite direction (y/n)"
read Answer
if [ "${Answer}" != "y" ]; then exit 1; fi
