/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: DHCPConst.cpp,v 1.3 2004-09-07 17:42:31 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 */
#include "DHCPConst.h"
/*                                                      Appearance of Options in Message Types

           Client Server IA_NA IA_TA IAAddr Option Pref  Time Relay Auth. Server  Status  Rap. User  Vendor Vendor Inter. Recon. Recon.
             ID     ID                      Request            Msg.       Unica.   Code  Comm. Class Class  Spec.    ID    Msg.  Accept
   Solicit   *             *     *             *           *           *                   *     *     *      *                    *
   Advert.   *      *      *     *                   *                 *             *           *     *      *                    *
   Request   *      *      *     *             *           *           *                         *     *      *                    *
   Confirm   *             *     *             *           *           *                         *     *      *
   Renew     *      *      *     *             *           *           *                         *     *      *                    *
   Rebind    *             *     *             *           *           *                         *     *      *                    *
   Decline   *      *      *     *             *           *           *                         *     *      *
   Release   *      *      *     *             *           *           *                         *     *      *
   Reply     *      *      *     *                   *                 *     *       *     *     *     *      *                    *
   Reconf.   *      *                          *                       *                                                    *
   Inform.   * (see note)                      *           *           *                         *     *      *                    *
   R-forw.                                                      *     *                          *     *      *      *
   R-repl.                                                      *     *                          *     *      *      *

*/

bool OptInMsg[13][24] = {
//         Client Server IA_NA IA_TA IAAddr Option Pref  Time Relay   Empty Auth. Server  Status  Rap. User  Vendor Vendor Inter. Recon. Recon. DNS    Domain NTP    Time
//           ID     ID                      Request            Msg.   Empty       Unica.   Code  Comm. Class Class  Spec.    ID    Msg.  Accept Server Name   Server Zone
// Solicit   *             *     *             *           *                   *                   *     *     *      *                    *      *       *     *     *
         {true , false, true,true, false, true,false,true,false, false,true,false, false,true, true, true, true, false, false,true , true,  true, true, true},
// Advert.   *      *      *     *                   *                         *             *           *     *      *                    *      *       *     *     *
         {true , true , true,true, false,false,true ,false,false,false,true,false,true ,false,true, true, true, false, false,true , true,  true, true, true},
// Request   *      *      *     *             *           *                   *                         *     *      *                    *      *       *     *     *
         {true , true , true,true, false, true,false,true,false, false,true,false, false,false,true, true, true, false, false,true , true,  true, true, true},                
// Confirm   *             *     *             *           *                   *                         *     *      *                                         *     *
         {true ,false , true,true, false, true,false,true,false, false,true,false, false,false,true, true, true, false, false,false, false,false, true, true},
// Renew     *      *      *     *             *           *                   *                         *     *      *                    *      *       *     *     *
         {true , true , true,true, false, true,false,true,false, false,true,false, false,false,true, true, true, false, false, true, true,  true, true, true},
// Rebind    *             *     *             *           *                   *                         *     *      *                    *      *       *     *     *
         {true ,false , true,true, false,    true,false,true,false, false,true,false, false,false,true, true, true, false, false, true, true,  true, true, true},
// Reply     *      *      *     *                   *                         *     *       *     *     *     *      *                    *      *       *     *     *
         {true ,true  , true,true, false,false,true,false,false, false,true, true, true ,true ,true, true, true, false, false,true , true,  true, true, true},
// Release   *      *      *     *             *           *                   *                         *     *      *
         {true ,true  , true,true, false, true,false,true,false, false,true,false, false,false,true, true, true, false, false,false, false,false,false,false},
// Decline   *      *      *     *             *           *                   *                         *     *      *
         {true ,true  , true,true, false, true,false,true,false, false,true,false, false,false,true, true, true, false, false,false, false,false,false,false},
// Reconf.   *      *                           *                               *                                                    *
         {true ,true  ,false,false,false,true,false,false,false, false,true,false,false,false,false,false,false,false, true ,false, false,false,false,false},
// Inform.   * (see note)                      *           *                   *                         *     *      *                    *      *       *     *     *
         {true , true,false,false ,false,true ,false,true,false, false,true,false, false,false,true, true, true, false, false,true , true,  true, true, true},
// R-forw.                                                       *             *                         *     *      *      *
         {false,false,false,false ,false,false,false,false,true, false,true,false, false,false,true, true, true, true , false,false, false,false,false,false},
// R-repl.                                                       *             *                      Q   *     *      *      *
         {false,false,false,false ,false,false,false,false,true, false,true,false, false,false,true, true, true, true , false,false, false,false,false,false},
};

bool canBeOptInMsg(int msgType, int optType)
{
    switch (optType)
    {
        case OPTION_DNS_RESOLVERS:  optType=21; break;
        case OPTION_DOMAIN_LIST:    optType=22; break;
        case OPTION_NTP_SERVERS:    optType=23; break;
        case OPTION_TIME_ZONE:      optType=24; break;
    }
    if (optType<11) 
	return OptInMsg[msgType-1][optType-1];
    return OptInMsg[msgType-1][optType];
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

bool canBeOptInOpt(int msgType, int optOut, int optIn)
{
    if ((msgType==RELAY_FORW)||(msgType==RELAY_REPL))
    {
    //FIXME:case OPTION_REL I dont't know what is these Relay Forw and Raly Reply
    //      There are no such options (there are such messages) so ??????????????
    }
    else
    {
        switch (optOut)
        {
            case 0: //Option Field
                if ((optIn!=OPTION_IAADDR)&&
                    (optIn!=OPTION_RELAY_MSG)&&
                    (optIn!=OPTION_INTERFACE_ID))
                    return true;
                break;
            case OPTION_IA:
            case OPTION_IA_TA:
                if ((optIn==OPTION_IAADDR)||(optIn==OPTION_STATUS_CODE))
                    return true;
                break;
            case OPTION_IAADDR:
                if (optIn==OPTION_STATUS_CODE)
                    return true;
                break;
        }
    }
    return false;
}
