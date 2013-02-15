#!/bin/sh

ServiceID=`echo show State:/Network/Global/IPv6 | /usr/sbin/scutil | awk '/PrimaryService/ { print $3 }'`

echo $ServiceID

echo "open" > /tmp/resolv.conf-dhcp
echo "d.init" >> /tmp/resolv.conf-dhcp
echo "d.add ServerAddresses * $SRV_OPTION23" >> /tmp/resolv.conf-dhcp
echo "d.add DomainName $SRV_OPTION24" >> /tmp/resolv.conf-dhcp
echo "set State:/Network/Service/$ServiceID/DNS" >> /tmp/resolv.conf-dhcp
echo "quit" >> /tmp/resolv.conf-dhcp

/usr/sbin/scutil < /tmp/resolv.conf-dhcp
