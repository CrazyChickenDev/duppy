RED="\033[1;31m"
GREEN="\e[32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
NORMAL="\033[0m"

SELECTED_INTERFACE=("")
SELECTED_MAC=("")

check_interface() {

    if [ -z $1 ]
    then
        echo
        echo -e "${RED}No interface selected! ${NORMAL}"
        echo
        exit
    fi

    if [ -z $(ifconfig | grep $1 | awk '{print $1}') ]
    then
        echo
        echo -e "${RED}Not a valid interface: '$1' ${NORMAL}"
        echo
        exit
    else
        echo
        echo -e "${YELLOW}Selected interface: '$1' ${NORMAL}"
        SELECTED_INTERFACE=("$1")
        echo
    fi

}

check_mac() {

    if [ -z $1 ]
    then
        echo
        echo -e "${RED}MAC address didnt specified! ${NORMAL}"
        echo -e "${YELLOW}Usage: --mac <address> ${NORMAL}"
        echo
        exit
    else
        echo -e "${YELLOW}Selected mac: '$1' ${NORMAL}"
        SELECTED_MAC=("$1")
    fi

}

check_duppy() {

    echo -e -n "${YELLOW}Duppy: ${NORMAL}"
    #check if duppy is running
    if [ -e /tmp/duppy ]
    then
        echo -e "${GREEN}\t\tENABLED${NORMAL}"
    else
        echo -e "${RED}\t\tDISABLED${NORMAL}"
    fi

    echo -e "${YELLOW}Hostname:\t${BLUE}$(hostname)${NORMAL}"

    ACTUAL_MAC=$(macchanger --show $SELECTED_INTERFACE | sed -n 1p | awk '{print $3}')
    PERMANENT_MAC=$(macchanger --show $SELECTED_INTERFACE | sed -n 2p | awk '{print $3}')

    echo -n -e "${YELLOW}Mac:\t\t${NORMAL}"
    if [ $ACTUAL_MAC = $PERMANENT_MAC ]
    then
        echo -e "${RED}$ACTUAL_MAC ${NORMAL}"
    else
        echo -e "${GREEN}$ACTUAL_MAC ${NORMAL}"
    fi

}

start() {

    echo -n -e "${YELLOW}Create folder:\t\t"
    mkdir /tmp/duppy && mkdir /tmp/duppy/known_ap
    if [ $? -eq "0" ]
    then
        echo "${GREEN}SUCCESS${NORMAL}"
    else
        echo "${RED}FAILED${NORMAL}"
    fi

    echo -n -e "${YELLOW}Save default hostname:\t"
    hostname > /tmp/duppy/defaulthostname
    if [ $? -eq "0" ]
    then
        echo "${GREEN}SUCCESS${NORMAL}"
    else
        echo "${RED}FAILED${NORMAL}"
    fi

    echo -n -e "${YELLOW}Stop network-manager:\t"
    service network-manager stop
    if [ $? -eq "0" ]
    then
        echo "${GREEN}SUCCESS${NORMAL}"
    else
        echo "${RED}FAILED${NORMAL}"
    fi

    echo -n -e "${YELLOW}Interface $SELECTED_INTERFACE set down:\t"
    ifconfig $SELECTED_INTERFACE down
    if [ $? -eq "0" ]
    then
        echo "${GREEN}SUCCESS${NORMAL}"
    else
        echo "${RED}FAILED${NORMAL}"
    fi

    echo -n -e "${YELLOW}Change mac address:\t"
    if [ -z $SELECTED_MAC ]
    then
        macchanger -r $SELECTED_INTERFACE
        if [ $? -eq "0" ]
        then
            echo "${GREEN}SUCCESS${NORMAL}"
        else
            echo "${RED}FAILED${NORMAL}"
        fi
    else
        macchanger -m $SELECTED_MAC $SELECTED_INTERFACE
        if [ $? -eq "0" ]
        then
            echo "${GREEN}SUCCESS${NORMAL}"
        else
            echo "${RED}FAILED${NORMAL}"
        fi
    fi

    echo -n -e "${YELLOW}Change hostname:\t"
    DEFAULT_HOSTNAME=$(cat /tmp/duppy/defaulthostname)
    HOSTNAME=$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c 20)
    MIT_COOKIE=$(xauth list | awk '{print $3}')

    echo $HOSTNAME > /proc/sys/kernel/hostname && \
    echo $HOSTNAME > /etc/hostname && \
    sed -i s/${DEFAULT_HOSTNAME}/${HOSTNAME}/g /etc/hosts && \
    xauth add ${HOSTNAME}/unix:0 . $MIT_COOKIE && \
    xauth remove $DEFAULT_HOSTNAME/unix:0
    if [ $? -eq "0" ]
    then
        echo "${GREEN}SUCCESS${NORMAL}"
    else
        echo "${RED}FAILED${NORMAL}"
    fi
    
}

#check uid
if [ $EUID -ne "0" ]
then
    echo -e "${RED}Run as root! ${NORMAL}"
    exit
fi

#parse arguments
while [ $# -gt 0 ]
do
    case $1 in
        --start)
            START=("1")
            check_interface $2
            shift 2
            ;;
        --stop)
            STOP=("1")
            check_interface $2
            shift 2
            ;;
        --check)
            CHECK=("1")
            check_interface $2
            shift 2
            ;;
        --mac)
            check_mac $2
            shift 2
            ;;
        --tor)
            TOR=("1")
            shift
            ;;
        --vpn)
            VPN=("1")
            shift
            ;;
        -h | --help)
            echo
            echo "  --start <interface>         Start Duppy"
            echo "  --stop <interface>          Stop Duppy"
            echo "  --check <interface>         Check if Duppy is running"
            echo "  --mac <address>             Set specific MAC address"
            echo "                              If you want random MAC address,"
            echo "                              just don't use '--mac' argument"
            echo "  --tor                       Start TOR with Duppy"
            echo "  --vpn                       Start VPN with Duppy"
            echo
            exit
            ;;
        *)
            echo
            echo -e "Invalid argument: ${RED}$1 ${NORMAL}"
            echo -e "Use '-h' for help"
            echo
            exit
            ;;
    esac
done


if [ $START ]
then
    start
elif [ $STOP ]
then
    echo "stop"
elif [ $CHECK ]
then
    check_duppy
fi
