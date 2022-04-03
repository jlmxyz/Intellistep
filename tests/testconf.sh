export MotorSerial=/dev/ttyUSB0
export BaudRate=115200


openSerial() 
{
    stty -F ${MotorSerial} ${BaudRate}
    export LOGFILE=$(date +%y-%m-%d.%T).$(basename "$0").log
    echo '##### start of test' | tee ${LOGFILE}
    exec 4<${MotorSerial}
    exec 5>${MotorSerial}
}

closeSerial()
{
    echo "##### end of test" | tee -a ${LOGFILE}
    exec 4<&-
    exec 5<&-
}

sendSerial()
{    
    echo "##### $@" | tee -a ${LOGFILE}
    echo "$@" >&5
}

readSerial()
{
    res=""
    _res="XXXX"
    while  [ "${_res}" != "${IFS}" ]; do
        read -r _res <&4
        if [ -n "${res}" ] && [ -n "${_res}" ]; then
            res="${res}${IFS}${_res}"
        elif [ -z "${_res}" ]; then
            _res="${_res}${IFS}"
        else
            res=${_res}
        fi 
    done
    sleep 0.5
    echo "${res}" | tee -a ${LOGFILE}
}
