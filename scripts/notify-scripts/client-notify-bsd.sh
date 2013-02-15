#!/usr/local/bin/bash

# author: Sly Midnight
# This script was tested on OpenBSD, but it is likely to work on other BSDs as well.

version="v0.2.3-openbsd"
LOGFILE=/var/lib/dibbler/client-notify-bsd.log

# uncomment this to get full list of available variables
set >> $LOGFILE
echo "Argument to script: $1 version: $version" >> $LOGFILE

echo "-----------" >> $LOGFILE

if [ -n "$OPTION_NEXT_HOP" ]; then

    route -n delete -inet6 default > /dev/null 2>&1
    route -n add -inet6 default ${OPTION_NEXT_HOP}%$IFACE
    echo "Added default route via ${OPTION_NEXT_HOP} on interface $IFACE/$IFINDEX" >> $LOGFILE

fi

if [ -n "$OPTION_NEXT_HOP_RTPREFIX" ]; then

    NEXT_HOP=`echo ${OPTION_NEXT_HOP_RTPREFIX} | awk '{print $1}'`
    NETWORK=`echo ${OPTION_NEXT_HOP_RTPREFIX} | awk '{print $2}'`
    #LIFETIME=`echo ${OPTION_NEXT_HOP_RTPREFIX} | awk '{print $3}'`
    METRIC=`echo ${OPTION_NEXT_HOP_RTPREFIX} | awk '{print $4}'`

    if [ "$NETWORK" == "::/0" ]; then

        route -n delete -inet6 default > /dev/null 2>&1
        route -n add -inet6 default ${OPTION_NEXT_HOP}%$IFACE
        echo "Added default route via  ${OPTION_NEXT_HOP} on interface $IFACE/$IFINDEX" >> $LOGFILE

    else

        route -n add -inet6 ${NETWORK} ${NEXT_HOP}%$IFACE
        echo "Added nexthop to network ${NETWORK} via ${NEXT_HOP} on interface $IFACE/$IFINDEX, metric ${METRIC}" >> $LOGFILE

    fi

fi

if [ -n "$OPTION_RTPREFIX" ]; then

    ONLINK=`echo ${OPTION_RTPREFIX} | awk '{print $1}'`
    METRIC=`echo ${OPTION_RTPREFIX} | awk '{print $3}'`
    route -n add -inet6 ${ONLINK}%$IFACE -iface
    echo "Added route to network ${ONLINK} on interface $IFACE/$IFINDEX onlink, metric ${METRIC}" >> $LOGFILE

fi


if [ -n "$ADDR1" ]; then
    echo "Address ${ADDR1} (operation $1) to client $REMOTE_ADDR on inteface $IFACE/$IFINDEX" >> $LOGFILE
    ifconfig $IFACE inet6 ${ADDR1} prefixlen 64
    default_route=`echo -n ${ADDR1} | awk -F\: '{print $1":"$2":"$3":"$4"::1"; }'`
    route -n delete -inet6 default
    route -n add -inet6 default ${default_route}
    echo "Added default route via ${default_route} on interface $IFACE/$IFINDEX" >> $LOGFILE

    if [ -n "$SRV_OPTION23" ]; then
        echo ${SRV_OPTION23} | awk -F' ' '{ print "nameserver "$1"\nnameserver "$2; }' >> /etc/resolv.conf
        cat /etc/resolv.conf | sort -u > /etc/resolv.tmp
        mv -f /etc/resolv.tmp /etc/resolv.conf
        echo "DNS servers ${SRV_OPTION23} (operation $1) to client $REMOTE_ADDR on inteface $IFACE/$IFINDEX" >> $LOGFILE
fi

fi

if [ -n "$PREFIX1" ]; then
    echo "Prefix ${PREFIX1} (operation $1) to client $REMOTE_ADDR on inteface $IFACE/$IFINDEX" >> $LOGFILE
    PREFIXIFACE=`cat /etc/dibbler/client.conf | grep -i downlink | grep -v -e '^#' | awk -F" " '{ print $2; }' | sed 's/\"//g'`
    ifconfig $PREFIXIFACE inet6 "$PREFIX1"1 prefixlen $PREFIX1LEN
fi

# sample return code. Dibbler will just print it out.
exit 3
