#!/bin/bash

LOGFILE=/var/lib/dibbler/client.sh-log

# uncomment this to get full list of available variables
#set >> $LOGFILE

echo "-----------" >> $LOGFILE

if [ "$OPTION_NEXT_HOP" != "" ]; then
    OPTION_NEXT_HOP=${OPTION_NEXT_HOP//\"/}

    #if [ "$SRV_OPTION243" == "" ] || [ $SRV_OPTION243 == "::/0"]; then
	ip -6 route del default
	ip -6 route add default via ${OPTION_NEXT_HOP} dev $IFACE
	echo "Added default route via ${OPTION_NEXT_HOP} on interface $IFACE/$IFINDEX" >> $LOGFILE

    #fi

fi

exit 3
#!/bin/bash

# this is example notify script that can be invoked on a client side
# This script will be called by client-server with a single parameter
# describing operation (add, update, delete, expire)
#
# Many parameters will be passed as environment variables

LOGFILE=/var/lib/dibbler/client-notify.log

echo "---$1--------" >> $LOGFILE
date >> $LOGFILE

# uncomment this to get full list of available variables
#set >> $LOGFILE

if [ "$ADDR1" != "" ]; then
    echo "Address ${ADDR1} (operation $1) to client $REMOTE_ADDR on inteface $IFACE/$IFINDEX" >> $LOGFILE
fi

if [ "$PREFIX1" != "" ]; then
    echo "Prefix ${PREFIX1} (operation $1) to client $REMOTE_ADDR on inteface $IFACE/$IFINDEX" >> $LOGFILE
fi

# sample return code. Dibbler will just print it out.
exit 3
