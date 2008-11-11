/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * $Id: DHCPConst.cpp,v 1.19 2008-11-11 22:37:03 thomson Exp $
 *
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */
#include "DHCPConst.h"
#include "Logger.h"
#include "SmartPtr.h"
#include <cstdio>

// standard options specified in RFC3315, pg. 99
bool OptInMsg[13][20] = {
//           Client Server  IA_NA IA_TA IAAddr Option  Pref  Elap  Relay  Empty Auth. Server  Status  Rap. User  Vendor Vendor Inter. Recon. Recon.
//             ID     ID                       Request       Time  Msg.   Empty       Unica.   Code  Comm. Class Class  Spec.    ID    Msg.  Accept
/*Solicit */ {true,  false, true,  true, false, true, false, true, false, false,true, false, false,  true, true, true,  true, false, false,true },
/*Advertise*/{true,  true , true,  true, false, false,true , false,false, false,true, true,  true ,  false,true, true,  true, false, false,true },
/*Request*/  {true,  true , true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false,true },
/*Confirm*/  {true,  false, true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false,false},
/*Renew*/    {true,  true , true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false, true},
/*Rebind*/   {true,  false, true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false, true},
/*Reply*/    {true,  true , true,  true, false, false,true,  false,false, false,true, true,  true ,  true ,true, true,  true, false, false,true },
/*Release*/  {true,  true , true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false,false},
/*Decline*/  {true,  true , true,  true, false, true, false, true, false, false,true, false, false,  false,true, true,  true, false, false,false},
/*Reconf.*/  {true,  true , false, false,false, true, false, false,false, false,true, false, false,  false,false,false, false,false, true ,false},
/*Inform.*/  {true,  true,  false, false,false, true ,false, true, false, false,true, false, false,  false,true, true,  true, false, false,true},
/*R-forw.*/  {false, false, false, false,false, false,false, false,true,  false,true, false, false,  false,true, true,  true, true , false,false},
/*R-repl.*/  {false, false, false, false,false, false,false, false,true,  false,true, false, false,  false,true, true,  true, true , false,false},
};

int allowOptInMsg(int msg, int opt)
{
    // standard options specified in RFC3315
    if (msg>13)
	return 1; // allow everthing in new messages
    if (opt <=20) {
	    return OptInMsg[msg-1][opt-1];
    }

    // additional options: allow them
    return 1;
}

/*          Appearance of Options in the Options Field of DHCP Options

                Option  IA_NA/ IAADDR Relay  Relay
                Field   IA_TA         Forw.  Reply
1  Client ID      *
2  Server ID      *
3  IA_NA          *
4  IA_TA          *
5  IAADDR                 *
6  ORO            *
7  Preference     *
8  Elapsed Time   *
9  Relay Message                        *      *
11 Authentic.     *
12 Server Uni.    *
13 Status Code    *       *       *
14 Rapid Comm.    *
15 User Class     *
16 Vendor Class   *
17 Vendor Info.   *
18 Interf. ID                           *      *
19 Reconf. MSG.   *
20 Reconf. Accept *
*/

int allowOptInOpt(int msgType, int parent, int subopt) {

    // additional options (not specified in RFC3315)
    if (subopt>20)
	return 1;

    if ((msgType==RELAY_FORW_MSG)||(msgType==RELAY_REPL_MSG)) {
	    if ( (subopt==OPTION_INTERFACE_ID) || (subopt=OPTION_RELAY_MSG))
	        return 1;
	    return 0;
    }

    switch (parent) {
    case 0: //Option Field
	if ((subopt!=OPTION_IAADDR)&&
	    (subopt!=OPTION_RELAY_MSG)&&
	    (subopt!=OPTION_INTERFACE_ID))
	    return 1;
	break;
    case OPTION_IA_NA:
    case OPTION_IA_TA:
	if ((subopt==OPTION_IAADDR)||(subopt==OPTION_STATUS_CODE))
	    return 1;
	break;
    case OPTION_IAADDR:
	if (subopt==OPTION_STATUS_CODE)
	    return 1;
	if (subopt==OPTION_ADDRPARAMS)
	    return 1;
	break;
    case OPTION_IA_PD:
	if ( (subopt==OPTION_IAPREFIX) || (subopt==OPTION_STATUS_CODE))
	    return 1;
    case OPTION_LQ_QUERY:
      if ( (subopt == OPTION_IAADDR) || (subopt==OPTION_CLIENTID) )
	return 1;
    }
    return 0;
}

unsigned DIGESTSIZE[] = {
        0,  //NONE
        32, //PLAIN
        16, //HMAC-MD5
        20, //HMAC-SHA1
        28, //HMAC-SHA224
        32, //HMAC-SHA256
        48, //HMAC-SHA384
        64, //HMAC-SHA512
        0   //_END_
};

char *DIGESTNAME[] = {
    (char *)"NONE",
    (char *)"PLAIN",
    (char *)"HMAC-MD5",
    (char *)"HMAC-SHA-1",
    (char *)"HMAC-SHA-224",
    (char *)"HMAC-SHA-256",
    (char *)"HMAC-SHA-384",
    (char *)"HMAC-SHA-512",
    (char *)""   //_END_
};

unsigned getDigestSize(enum DigestTypes type) {
        if (type >= DIGEST_INVALID) {
                Log(Error) << "Invalid digest type: " << type << LogEnd;
                return 0;
        }
        return DIGESTSIZE[type];
}

char *getDigestName(enum DigestTypes type) {
    return DIGESTNAME[type];
}

void PrintHex(std::string message, char *buffer, unsigned len) {
    unsigned j;
    char *buf = new char[len*3+1];

    if (len) {
        for (j = 0; j < len; j++) {
            sprintf(buf + j*3, "%02x ", (unsigned char) *(buffer+j));
        }

        Log(Debug) << message << buf << LogEnd;
    } else 
        Log(Debug) << message << "N/A (zero length)" << LogEnd;

    delete buf;
}

