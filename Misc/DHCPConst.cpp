/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: DHCPConst.cpp,v 1.7 2006-10-06 00:25:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2004/12/08 01:08:51  thomson
 * OptInMsg value corrected.
 *
 * Revision 1.4  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.3  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 */
#include "DHCPConst.h"


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

bool allowOptInMsg(int msg, int opt)
{
    // standard options specified in RFC3315
    if (opt <=20) {
	return OptInMsg[msg-1][opt-1];
    }

    // additional options: allow them
    return true;
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

bool allowOptInOpt(int msgType, int parent, int subopt) {

    // additional options (not specified in RFC3315)
    if (subopt>20)
	return true;

    if ((msgType==RELAY_FORW_MSG)||(msgType==RELAY_REPL_MSG)) {
	if ( (subopt==OPTION_INTERFACE_ID) || (subopt=OPTION_RELAY_MSG))
	    return true;
	return false;
    }

    switch (parent) {
    case 0: //Option Field
	if ((subopt!=OPTION_IAADDR)&&
	    (subopt!=OPTION_RELAY_MSG)&&
	    (subopt!=OPTION_INTERFACE_ID))
	    return true;
	break;
    case OPTION_IA:
    case OPTION_IA_TA:
	if ((subopt==OPTION_IAADDR)||(subopt==OPTION_STATUS_CODE))
	    return true;
	break;
    case OPTION_IAADDR:
	if (subopt==OPTION_STATUS_CODE)
	    return true;
	break;
    }
    return false;
}
