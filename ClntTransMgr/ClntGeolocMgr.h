/* 
 * File:   ClntGeolocMgr.h
 * Author: Michal Golon
 */

#ifndef CLNTGEOLOCMGR_H
#define	CLNTGEOLOCMGR_H

#include "Portable.h"
#include "Logger.h"
#include "SmartPtr.h"
#include "Container.h"
#include <iostream>
#include <string.h>

class TClntGeolocMgr {
public:
    TClntGeolocMgr();
    
    static int checkClntConf(const string cfgFile);
    static List(string) getCoordinates(const string cfgFile);
    
    ~TClntGeolocMgr();
    
private:

};

#endif	/* CLNTGEOLOCMGR_H */

