/* 
 * File:   ClntGeolocMgr.cpp
 * Author: Michal Golon
 * 
 */

#include "ClntGeolocMgr.h"
#include "Portable.h"
#include "DHCPConst.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

TClntGeolocMgr::TClntGeolocMgr() {
}

/*
 * Check mode saved in client's configuration file.
 * 
 * If file doesn't exist, mode is 0.
 * If file exists, mode is written in second line of the file.
 */
int TClntGeolocMgr::checkClntConf(const string cfg) {  
    ifstream cfgFile(cfg.c_str(), ios::in);
    if(!cfgFile.is_open()) {
        return 0;
    }
    
    string description;
    string modeChar;
    
    getline(cfgFile, description);
    getline(cfgFile, modeChar);

    cfgFile.close();

    int mode = atoi(modeChar.c_str());
    
    return mode;
}

/*
 * Get coordinates from client's configuration file.
 * 
 * Coordinates begin from 4th row of the file.
 */
List(string) TClntGeolocMgr::getCoordinates(const string cfg) {   
    List(string) coordinatesList;
    
    ifstream cfgFile(cfg.c_str(), ios::in);
    if(!cfgFile.is_open()) {
        return coordinatesList;
    }
    
    string junk;
    string coordinateFromFile;

    SPtr<string> coordinate;
    
    getline(cfgFile, junk);
    getline(cfgFile, junk);
    getline(cfgFile, junk);
    for(int i = 0; i < 8; i++) {
        getline(cfgFile, coordinateFromFile);
        coordinate = new string(coordinateFromFile);
        coordinatesList.append(coordinate);
    }
    
    cfgFile.close();
    
    return coordinatesList;
}

TClntGeolocMgr::~TClntGeolocMgr() {
}

