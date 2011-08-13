/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: Requestor.cpp,v 1.7 2008-09-22 17:08:54 thomson Exp $
 */

#include <string.h>
#include <stdlib.h>
#include "Portable.h"
#include "ReqCfgMgr.h"
#include "Portable.h"
#include "IfaceMgr.h"
#include "ReqTransMgr.h"
#include "Logger.h"

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

using namespace std;

void printHelp() 
{
    cout << "Usage:" << endl
         << "-i IFACE - send query using iface inteface, e.g. -i eth0" << endl
         << "-addr ADDR - query about address, e.g. -addr 2000::43" << endl
         << "-duid DUID - query about DUID, e.g. -duid 00:11:22:33:44:55:66:77:88" << endl
	 << "-bulk ADDR - query about link-address, e.g. -bulk 2000::43" << endl
         << "-timeout 10 - query timeout, specified in seconds" << endl
	 << "-dstaddr 2000::1 - destination address (by default it is ff02::1:2)" << endl;
}

bool parseCmdLine(ReqCfgMgr *a, int argc, char *argv[])
{
    char * addr    = 0;
    char * duid    = 0;
    char * bulk    = 0;
    char * iface   = 0;
    char * dstaddr = 0;
    int timeout  = 60; // default timeout value
    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i],"-addr", 5)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -addr used, but actual address is missing." << LogEnd;
                return false;
            }
            addr = argv[++i];
            continue;
        }
        if (!strncmp(argv[i],"-duid", 5)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -duid used, but actual DUID is missing." << LogEnd;
                return false;
            }
            duid = argv[++i];
            continue;
        }
        if (!strncmp(argv[i], "-bulk", 5)) {
            if (argc == i) {
                Log(Error) << "Unable to parse command-line. -bulk used, but actual link-address is missing." << LogEnd;
                return false;
            }
            bulk = argv[++i];
            continue;
        }
        if (!strncmp(argv[i],"-i", 2)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -i used, but actual interface name is missing." << LogEnd;
                return false;
            }
            iface = argv[++i];
            continue;
        }
        if (!strncmp(argv[i],"-timeout", 8)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -timeout used, but actual timeout value is missing." << LogEnd;
                return false;
            }
            timeout = atoi(argv[++i]);            
            continue;
        }
	if (!strncmp(argv[i],"-dstaddr", 7)) {
	    if (argc==i) {
		Log(Error) << "Unable to parse command-line. -dstaddr used, but actual destination address is missing." << LogEnd;
	    }
	    dstaddr = argv[++i];
	    continue;
	}
        if (!strncmp(argv[i], "--help", 5) || !strncmp(argv[i], "-h", 5) || !strncmp(argv[i], "/help", 5) || 
	    !strncmp(argv[i], "-?", 2) || !strncmp(argv[i], "/?",2)) {
            return false;
        }
        
        Log(Error) << "Unable to parse command-line parameter: " << argv[i] << LogEnd;
        Log(Error) << "Please use -h for help." << LogEnd;
        return false;
    }
/*
    if (!addr && !duid) {
        Log(Error) << "Both address and DUID not defined." << LogEnd;
        return false;
    }
    if (addr && duid) {
        Log(Error) << "Both address and DUID defined." << LogEnd;
        return false;
    }
*/
    if (!iface) {
        Log(Error) << "Interface not defined. Please use -i command-line switch." << LogEnd;
        return false;
    }

    a->addr  = addr;
    a->duid  = duid;
    a->bulk  = bulk;
    a->iface = iface;
    a->timeout= timeout;
    a->dstaddr = dstaddr;
    return true;
}

int initWin()
{
#ifdef WIN32
    WSADATA wsaData;
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData )) {
        cout << "Unable to load WinSock 2.2 library." << endl;
		return -1;
	}
#endif
    return 0;
}

int main(int argc, char *argv[])
{
    // int status = 0;
    ReqCfgMgr a;
    //memset(&a, 0 sizeof(a));

    initWin();

    // srandom(time(NULL)); // Linux
    srand(time(NULL));      // Windows

    logger::setLogName("Requestor");
	logger::Initialize((char*)REQLOG_FILE);

	cout << DIBBLER_COPYRIGHT1 << " (REQUESTOR)" << endl;
	cout << DIBBLER_COPYRIGHT2 << endl;
	cout << DIBBLER_COPYRIGHT3 << endl;
	cout << DIBBLER_COPYRIGHT4 << endl;
	cout << endl;

    if (!parseCmdLine(&a, argc, argv)) {
        Log(Crit) << "Aborted. Invalid command-line parameters or help called." << LogEnd;
        printHelp();
        return -1;
    }

    TIfaceMgr   * ifaceMgr = new TIfaceMgr(REQIFACEMGR_FILE, true);
    ReqTransMgr * transMgr = new ReqTransMgr(ifaceMgr);

    transMgr->SetParams(&a);

    if (!transMgr->BindSockets()) {
        Log(Crit) << "Aborted. Socket binding failed." << LogEnd;
        return LOWLEVEL_ERROR_BIND_FAILED;
    }
 
    if (!transMgr->SendMsg()) {
        Log(Crit) << "Aborted. Message transmission failed." << LogEnd;
        return LOWLEVEL_ERROR_SOCKET;
    }
    if (!transMgr->WaitForRsp()) {


    }

    free(transMgr);

    return LOWLEVEL_NO_ERROR;
}


/* linker workarounds: dummy functions */
extern "C"
{
    void *hmac_sha (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type) { return 0; }
    void *hmac_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf) { return 0; }
}

#ifndef WIN32
//unsigned getDigestSize(enum DigestTypes type) { return 0; }
#endif
