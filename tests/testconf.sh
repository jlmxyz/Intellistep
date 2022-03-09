export MotorSerial=/dev/ttyUSB0


openSerial() 
{
    exec 3<>${MotorSerial}
}

closeSerial()
{
    exec 3<>&- 
}

sendSerial()
{
    echo $@ >&3
}

readSerial()
{
    buff=$(read <&3)
    return ${buff}
}
