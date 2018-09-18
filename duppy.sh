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

    if [ -z $(ifconfig | grep -e "$1" | awk '{print $1}') ]
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
        echo -e "${RED}MAC address didn't specified! ${NORMAL}"
        echo -e "${YELLOW}Usage: --mac <address> ${NORMAL}"
        echo
        exit
    else
        echo -e "${YELLOW}Selected mac: '$1' ${NORMAL}"
        SELECTED_MAC=("$1")
    fi

}

check_duppy() {

    #check if duppy is running
    echo -e -n "${YELLOW}Duppy: ${NORMAL}"
    if [ -e /tmp/duppy ]
    then
        echo -e "${GREEN}\t\tENABLED${NORMAL}"
    else
        echo -e "${RED}\t\tDISABLED${NORMAL}"
    fi

    #check hostname
    #if no /tmp/duppy/defaulthostname find, the print in RED
    #if ../defaulthostname equals to the actual hostname, then print in RED
    #else: the actual hostname is not equals the original
    echo -n -e "${YELLOW}Hostname:\t"
    if [ ! -e /tmp/duppy/defaulthostname ]
    then
        echo -e "${RED}$(hostname) ${NORMAL}"
    else
        if [ $(cat /tmp/duppy/defaulthostname) = $(hostname) ]
        then
            echo -e "${RED}$(hostname) ${NORMAL}"
        else
            echo -e "${GREEN}$(hostname) ${NORMAL}"
        fi
    fi

    ACTUAL_MAC=$(macchanger --show $SELECTED_INTERFACE | sed -n 1p | awk '{print $3}')
    PERMANENT_MAC=$(macchanger --show $SELECTED_INTERFACE | sed -n 2p | awk '{print $3}')

    echo -n -e "${YELLOW}Mac:\t\t${NORMAL}"
    if [ $ACTUAL_MAC = $PERMANENT_MAC ]
    then
        echo -e "${RED}$ACTUAL_MAC ${NORMAL}"
    else
        echo -e "${GREEN}$ACTUAL_MAC ${NORMAL}"
    fi
    echo

}

start() {

    if [ -e /tmp/duppy ]
    then
        echo -e "${RED}Duppy is started, so can't start! ${NORMAL}"
        echo
        exit
    fi

    #create temporary folder for duppy
    echo -n -e "${YELLOW}Create folder:\t\t\t\t"
    mkdir /tmp/duppy && mkdir /tmp/duppy/known_ap
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #save the default hostname for restore
    echo -n -e "${YELLOW}Save default hostname:\t\t\t"
    hostname > /tmp/duppy/defaulthostname
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #stop network-manager
    echo -n -e "${YELLOW}Stop network-manager:\t\t\t"
    service network-manager stop
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #set the SELECTED_INTERFACE down
    echo -n -e "${YELLOW}Interface $SELECTED_INTERFACE set down:\t\t"
    ifconfig $SELECTED_INTERFACE down
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #change MAC address
    #if MAC address not specified, then random generate a random one
    echo -n -e "${YELLOW}Change mac address:\t\t\t"
    if [ -z $SELECTED_MAC ]
    then
        #MAC address not specified
        macchanger -r $SELECTED_INTERFACE > /dev/null
        if [ $? -eq "0" ]
        then
            echo -e "${GREEN}SUCCESS${NORMAL}"
        else
            echo -e "${RED}FAILED${NORMAL}"
        fi
    else
        #MAC address specified
        macchanger -m $SELECTED_MAC $SELECTED_INTERFACE > /dev/null
        if [ $? -eq "0" ]
        then
            echo -e "${GREEN}SUCCESS${NORMAL}"
        else
            echo -e "${RED}FAILED${NORMAL}"
        fi
    fi

    sleep 1

    #change hostname
    #add the new hostname to xauth, and delete the old
    echo -n -e "${YELLOW}Change hostname:\t\t\t"
    DEFAULT_HOSTNAME=$(cat /tmp/duppy/defaulthostname)
    HOSTNAME=$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c 20)
    MIT_COOKIE=$(xauth -i -b list | awk '{print $3}')

    echo $HOSTNAME > /proc/sys/kernel/hostname && \
    echo $HOSTNAME > /etc/hostname && \
    sed -i s/${DEFAULT_HOSTNAME}/${HOSTNAME}/g /etc/hosts && \
    xauth -i -b add ${HOSTNAME}/unix:0 . $MIT_COOKIE && \
    xauth -i -b remove $DEFAULT_HOSTNAME/unix:0
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #ignore ARP packets
    echo -e -n "${YELLOW}Ignoring ARP:\t\t\t\t"
    sysctl -q -w net.ipv4.conf.${SELECTED_INTERFACE}.arp_ignore=8
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #ignore ARP announce
    echo -n -e "${YELLOW}Restrict ARP announce:\t\t\t"
    sysctl -q -w net.ipv4.conf.${SELECTED_INTERFACE}.arp_announce=2
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #disable response to ping probes
    echo -n -e "${YELLOW}Disable ping response:\t\t\t"
    sysctl -q -w net.ipv4.icmp_echo_ignore_all=1
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #temporary remove the known APs
    #makes impossible to reconnect by accident
    echo -n -e "${YELLOW}Temporary remove known APs:\t\t"
    mv -f /etc/NetworkManager/system-connections/* /tmp/duppy/known_ap/ 2> /dev/null
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #set up the SELECTED_INTERFACE
    echo -n -e "${YELLOW}Interface $SELECTED_INTERFACE set up:\t\t\t"
    ifconfig $SELECTED_INTERFACE up
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #start the network-manager
    echo -n -e "${YELLOW}Start network-manager:\t\t\t"
    service network-manager start
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #change ownership of Xauthority
    #need for the proper reboot
    echo -n -e "${YELLOW}Change owner of .Xauthority:\t\t"
    chown $(logname) /home/$(logname)/.Xauthority
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #change ownership of ICEauthority
    #need for the proper reboot
    echo -n -e "${YELLOW}Change owner of .ICEauthority:\t\t"
    chown $(logname) /home/$(logname)/.ICEauthority
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

}

stop() {

    if [ ! -e /tmp/duppy ]
    then
        echo -e "${RED}Duppy not started, so can't stop! ${NORMAL}"
        echo
        exit
    fi

    #stop network-manager
    echo -n -e "${YELLOW}Stop network-manager:\t\t\t"
    service network-manager stop
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #set down the SELECTED_INTERFACE
    echo -n -e "${YELLOW}Interface $SELECTED_INTERFACE set down:\t\t"
    ifconfig $SELECTED_INTERFACE down
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #set back the original MAC address
    echo -n -e "${YELLOW}Set back the original MAC address:\t"
    macchanger -p $SELECTED_INTERFACE > /dev/null
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #change hostname
    #add the default hostname to xauth, and delete the old
    echo -n -e "${YELLOW}Change hostname:\t\t\t"
    DEFAULT_HOSTNAME=$(cat /tmp/duppy/defaulthostname)
    HOSTNAME=$(hostname)
    MIT_COOKIE=$(xauth -i -b list | awk '{print $3}')

    echo $DEFAULT_HOSTNAME > /proc/sys/kernel/hostname && \
    echo $DEFAULT_HOSTNAME > /etc/hostname && \
    sed -i s/${HOSTNAME}/${DEFAULT_HOSTNAME}/g /etc/hosts && \
    xauth -i -b add ${DEFAULT_HOSTNAME}/unix:0 . $MIT_COOKIE && \
    xauth -i -b remove $HOSTNAME/unix:0
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #allow arp
    echo -n -e "${YELLOW}Allow ARP:\t\t\t\t"
    sysctl -q -w net.ipv4.conf.${SELECTED_INTERFACE}.arp_ignore=0
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #enable ARP announce
    echo -n -e "${YELLOW}Enable ARP announce:\t\t\t"
    sysctl -q -w net.ipv4.conf.${SELECTED_INTERFACE}.arp_announce=0
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #enable ICMP answer
    echo -n -e "${YELLOW}Enable ping response:\t\t\t"
    sysctl -q -w net.ipv4.icmp_echo_ignore_all=0
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    echo -n -e "${YELLOW}Remove the used APs:\t\t\t"
    rm -f /etc/NetworkManager/system-connections/*
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    echo -n -e "${YELLOW}Set back know APs:\t\t\t"
    mv -f /tmp/duppy/known_ap/* /etc/NetworkManager/system-connections/ 2> /dev/null
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    echo -e -n "${YELLOW}Interface $SELECTED_INTERFACE set up:\t\t\t"
    ifconfig $SELECTED_INTERFACE up
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    echo -n -e "${YELLOW}Start network-manager:\t\t\t"
    service network-manager start
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #remove the temporary folder
    echo -n -e "${YELLOW}Remove duppy's folder:\t\t\t"
    rm -rf /tmp/duppy
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    #change ownership of Xauthority
    #need for the proper reboot
    echo -n -e "${YELLOW}Change owner of .Xauthority:\t\t"
    chown $(logname) /home/$(logname)/.Xauthority
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

    #change ownership of ICEauthority
    #need for the proper reboot
    echo -n -e "${YELLOW}Change owner of .ICEauthority:\t\t"
    chown $(logname) /home/$(logname)/.ICEauthority
    if [ $? -eq "0" ]
    then
        echo -e "${GREEN}SUCCESS${NORMAL}"
    else
        echo -e "${RED}FAILED${NORMAL}"
    fi

    sleep 1

}

if [ $# -eq "0" ]
then
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
fi

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
    stop
elif [ $CHECK ]
then
    check_duppy
fi
