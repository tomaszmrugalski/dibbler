/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 * help desk: Asia Czerniak
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <string.h>
#include <stdlib.h>
#include "Portable.h"
#include "ReqCfgMgr.h"
#include "Portable.h"
#include "IfaceMgr.h"
#include "ReqTransMgr.h"
#include "Logger.h"
#include "DHCPConst.h"

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
     //<< "-bulk ADDR - query about link-address, e.g. -bulk 2000::43" << endl
     << "To send bulk (multiple) query:"<< endl
     << "- bulk -dstaddr -m ADDRESS [] CLIENT_ID [] LINK_ADDRESS [] RELAY_ID [] REMOTE_ID [], e.g. -bulk -m 2000::43 00:11:22:33:44:55:66:77:88 00:11:22:33:88:44:55:66:77"<< endl
        << "-timeout 10 - query timeout, specified in seconds" << endl
        << "-dstaddr 2000::1 - destination address (by default it is ff02::1:2)" << endl;

}

int parseMultiQueryCmd(char * inputString ) {

    /*QUERY_BY_ADDRESS
    QUERY_BY_CLIENT_ID = 2,
    QUERY_BY_RELAY_ID = 3,
    QUERY_BY_LINK_ADDRESS = 4,
    QUERY_BY_REMOTE_ID = 5*/
     Log( Debug) << "LOGParseM" << LogEnd;
     Log( Debug) << "input string:" << inputString << LogEnd;

    if (!strncmp(inputString,"ADDRESS", 7)) {
        return 1;
    } else if(!strncmp(inputString,"CLIENT_ID", 9) ) {
        return 2;
    } else if(!strncmp(inputString,"RELAY_ID", 8) ){
        return 3;
    } else if(!strncmp(inputString,"LINK_ADDRESS", 12) ){
        return 4;
    } else if (!strncmp(inputString,"REMOTE_ID", 9) ){
        return 5;
    }
    return 0;
}
bool parseCmdLine(ReqCfgMgr *a, int argc, char *argv[])
{
    char * addr    = 0;
    char * duid    = 0;
    char * linkAddr =0;
    char * bulk    = 0;
    char * iface   = 0;
    char * dstaddr = 0;
    char * remoteId =0;
    char * relayId =0;
    int enterpriseNumber =0, tmpOptCode=0, requestCount=0;
    bool multiplyQ = false;


    int timeout  = 60; // default timeout value
    for (int i=1; i<=argc; i++) {
        //Log(Info) << "i: \t" << i << "\t argv: " << argv[i] << endl;
        if ((!strncmp(argv[i],"-addr", 5)) && (strlen(argv[i])==5)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -addr used, but actual address is missing." << LogEnd;
                return false;
            }
            addr = argv[++i];
            continue;
        }
        if ((!strncmp(argv[i],"-duid", 5)) && (strlen(argv[i])==5)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -duid used, but actual DUID is missing." << LogEnd;
                return false;
            }
            duid = argv[++i];
            continue;
        }
        if ((!strncmp(argv[i], "-bulk", 5)) && (strlen(argv[i])==5)) {

            if (argc == i) {
                Log(Error) << "Unable to parse command-line. -bulk used, but actual link-address is missing." << LogEnd;
                return false;
            }

            if ((!strncmp(argv[++i],"-m", 2)) && (strlen(argv[i])==2) ) {
                multiplyQ = true;
                ++i;
                while (i < argc) {

                    if ( strlen(argv[i]) < 5 ) {
                       continue;
                    }

                   // tmpOptCode = parseMultiQueryCmd(argv[i]);
                    Log( Debug) << "tmpCode" << tmpOptCode << LogEnd;
                    if(tmpOptCode>0) {
                        switch (tmpOptCode) {

                        case QUERY_BY_ADDRESS:
                            addr=argv[++i];
                            if (argc == i) {
                                Log(Error) << "Unable to parse command-line. -bulk used, but actual address is missing." << LogEnd;
                                return false;
                            } requestCount++;
                            break;

                        case QUERY_BY_CLIENT_ID:
                            duid=argv[++i];
                            if (argc == i) {
                                Log(Error) << "Unable to parse command-line. -bulk used, but actual DUID is missing." << LogEnd;
                                return false;
                            }requestCount++;
                            break;
                        case QUERY_BY_LINK_ADDRESS:
                            linkAddr=argv[++i];
                            if (argc == i) {
                                Log(Error) << "Unable to parse command-line. -bulk used, but actual link-addr is missing." << LogEnd;
                                return false;
                            }requestCount++;
                            break;
                        case QUERY_BY_RELAY_ID:
                            relayId = argv[++i];
                            if (argc == i) {
                                Log(Error) << "Unable to parse command-line. -bulk used, but actual relay is missing." << LogEnd;
                                return false;
                            }requestCount++;
                            break;
                        case QUERY_BY_REMOTE_ID:
                            remoteId = argv[++i];
                            if (argc == i) {
                                Log(Error) << "Unable to parse command-line. -bulk used, but actual remoteId is missing." << LogEnd;
                                return false;
                            }requestCount++;
                            break;

                         default:
                            break;

                        }
                      }
                    ++i;
                };

                //zeroing count of multiple request
                a->requestCount=requestCount;
                if (multiplyQ && !requestCount) {
                    Log(Error) << "Unable to parse command-line. -bulk used, but there is no more parameter to." << LogEnd;
                    return false;
                }
            }
            if (argc == i) {
                Log(Error) << "Unable to parse command-line. -bulk used, but actual remoteId is missing." << LogEnd;
                //return false;
            }
            bulk = argv[++i];
            continue;
        }

        if (i==argc)
           break;

        if ((!strncmp(argv[i],"-i", 2)) && ((strlen(argv[i])==2) )) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -i used, but actual interface name is missing." << LogEnd;
                return false;
            }
            iface = argv[++i];
            continue;
        }
        if ((!strncmp(argv[i],"-timeout", 8)) && (strlen(argv[i])==8)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -timeout used, but actual timeout value is missing." << LogEnd;
                return false;
            }
            timeout = atoi(argv[++i]);
            continue;
        }
        if ((!strncmp(argv[i],"-dstaddr", 8)) && (strlen(argv[i])==8) ) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -dstaddr used, but actual destination address is missing." << LogEnd;
            }
            dstaddr = argv[++i];
            Log(Info) << "DstAddr:" << dstaddr<< LogEnd;
            continue;
        }


        if ((!strncmp(argv[i],"-remoteId", 9)) && (strlen(argv[i])==9) ) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -remoteId used, but actual address is missing." << LogEnd;
                return false;
            }
            remoteId = argv[++i];
            continue;
        }

        if ((!strncmp(argv[i],"-enterpriseNumber", 17)) && (strlen(argv[i])==17)) {
            if (argc==i) {
                Log(Error) << "Unable to parse command-line. -enterpriseNumber used, but actual address is missing." << LogEnd;
                return false;
            }
            enterpriseNumber = atoi(argv[++i]);
            continue;
        }
        if ((!strncmp(argv[i], "--help", 6)&&(strlen(argv[i])==6)) || (!strncmp(argv[i], "-h", 2)&&(strlen(argv[i])==2)) || (!strncmp(argv[i], "/help", 5)&&(strlen(argv[i])==5))||
                (!strncmp(argv[i], "-?", 2)&&(strlen(argv[i])==2)) || (!strncmp(argv[i], "/?",2)&&(strlen(argv[i])==2)) ) {
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
    a->linkAddr = linkAddr;
    a->remoteId = remoteId;
    a->enterpriseNumber = enterpriseNumber;
    a->multiplyQuery = multiplyQ;


    Log(Debug) << "Addr:" << a->duid <<LogEnd;
    Log(Debug) << "Duid:" << a->duid <<LogEnd;
    Log(Debug) << "Bulk:" <<a->bulk << LogEnd;
    Log(Debug) << "Iface:" << a->iface << LogEnd;
    Log(Debug) << "Timeout:"<< a->timeout << LogEnd;
    Log(Debug) << "dstAddr"<< a->dstaddr <<LogEnd;
    Log(Debug) << "LinkAddr:"<< a->linkAddr << LogEnd;
    Log(Debug) << "remoteId:" << a->remoteId <<LogEnd;
    Log(Debug) << "enterpriseNumber"<<a->enterpriseNumber<< LogEnd;
    Log(Debug) << "MultipleQuery:"<< a->multiplyQuery<< LogEnd;
    return true;
}

int EmptyBulkQueue(ReqCfgMgr *a ) {
    /* a->addr  = addr;
    a->duid  = duid;
    a->bulk  = bulk;
    a->iface = iface;
    a->timeout= timeout;
    a->dstaddr = dstaddr;
    a->linkAddr = linkAddr;
    a->remoteId = remoteId;
    a->enterpriseNumber = enterpriseNumber;*/


    if (a->addr) {
        a->queryType = QUERY_BY_ADDRESS;
       // a->addr = 0;
        return 1;
    } else if (a->duid) {
        a->queryType = QUERY_BY_CLIENT_ID;
        return 1;
    } else if (a->enterpriseNumber) {
        a->queryType = QUERY_BY_RELAY_ID;
        return 1;
    } else if (a->remoteId) {
        a->queryType = QUERY_BY_REMOTE_ID;
        return 1;
    } else if (a->linkAddr) {
        a->queryType = QUERY_BY_LINK_ADDRESS;
        return 1;
    } else {
        Log(Debug) << "There is no more parameter to set by bulkQueueMgr" << endl;
        return 0;
    }

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
    int i=0;
    ReqCfgMgr a;

    initWin();

    srand(static_cast<unsigned int>(time(NULL))); // Windows

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

    //leasequery's part
    transMgr->SetParams(&a);
    /*if (!a.bulk) {
        if (!transMgr->BindSockets()) {
            Log(Crit) << "Aborted. Socket binding failed." << LogEnd;
            return LOWLEVEL_ERROR_BIND_FAILED;
        }


        if (!transMgr->SendMsg()) {
            Log(Crit) << "Aborted. Message transmission failed." << LogEnd;
            return LOWLEVEL_ERROR_SOCKET;
        }


        if (!transMgr->WaitForRsp()) {
            Log(Crit) << "Aborted. Cannot receive any data, WaitForResponse function failed." << LogEnd;
            return LOWLEVEL_ERROR_SOCKET;
        }

    } */

    //bulk's part
    //TODO: if statement
    if (a.multiplyQuery) {

        if(!transMgr->CreateNewTCPSocket()) {
            Log(Crit) << "Aborted. TCP socket creation failed." << LogEnd;
            return LOWLEVEL_ERROR_SOCKET;
        }

        Log(Debug) << "RequestCount:" << a.requestCount << LogEnd;
        while (i != a.requestCount ) {

            if (!EmptyBulkQueue(&a)) {
                Log(Debug) << "Cannot set queryType (EmptyBulkQueue failed)" << endl;
            } else {
                Log(Debug) <<  "query type has been set" << endl;
            }

            if(!transMgr->SendTcpMsg()) {
                Log(Crit) << "Aborted. TCP message transmission failed." << LogEnd;
                return LOWLEVEL_ERROR_SOCKET;
            }

            if (!transMgr->WaitForRsp()) {
                Log(Crit) << "Aborted. Cannot receive any data, WaitForResponse function failed." << LogEnd;
                //transMgr->RetryConnection();
                return LOWLEVEL_ERROR_SOCKET;
            }
        }

        transMgr->TerminateTcpConn();
    }

    delete transMgr;

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
