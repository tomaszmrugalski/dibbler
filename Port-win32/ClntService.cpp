#include "ClnService.h"
#include "DHCPClient.h"
#include "portable.h"
#include "logger.h"
#include "DHCPConst.h"
#ifdef  WIN32
#include <crtdbg.h>
#endif

char* ipv6Path;

TClntService::TClntService() : TWinService("DHCPv6Client","Klient DHCPv6",SERVICE_AUTO_START,"RpcSS\0tcpip6\0winmgmt\0")
{
}

bool TClntService::ParseStandardArgs(int argc,char* argv[])
{    
    if (argc>1)
    {
        bool dirFound=false;
        bool ipFound=true;
        int pres=1;
        //clog<<logger::logInfo<<"argv["<<pres<<"]="<<argv[pres]<<endl;
        while(argc>pres)
        {
            if (strncmp(argv[pres], "-d\"",3)==0)
            {
                char temp[255];
                temp[0]=0;
                strcat(temp,argv[pres]+3);
                while((pres<argc)&&
                      (argv[pres][strlen(argv[pres])-1]!='"'))
                {
                    pres++;
                    strcat(temp," ");
                    strcat(temp,argv[pres]);
                }
                temp[strlen(temp)-1]=0;                
                this->ServiceDir=temp;
                dirFound=true;     
            }
            if (strncmp(argv[pres], "-i\"",3)==0)
            {
                ipv6Path= new char [255];
                ipv6Path[0]=0;
                strcat(ipv6Path,argv[pres]+3);
                while((pres<argc)&&
                      (argv[pres][strlen(argv[pres])-1]!='"'))
                {
                    pres++;
                    strcat(ipv6Path," ");
                    strcat(ipv6Path,argv[pres]);
                }
                ipv6Path[strlen(ipv6Path)-1]=0;
                ipFound=true;
            }
            pres++;
        }
        return false;
    }
    return true;
}

void TClntService::OnShutdown()
{
    clog<<logger::logInfo<<"Service shutdown."<<std::endl;
    int fd=sock_add("",1,"::1",DHCPCLIENT_PORT+1005,true);
    char buf[4]={CONTROL_MSG,0,0,0};
    sock_send(fd,"::1",buf,4,DHCPCLIENT_PORT+1000,1);
    sock_del(fd);
}

void TClntService::OnStop()
{
    clog<<logger::logInfo<<"Stopping service."<<std::endl;
    int fd=sock_add("",1,"::1",DHCPCLIENT_PORT+1005,true);
    char buf[4]={CONTROL_MSG,0,0,0};
    sock_send(fd,"::1",buf,4,DHCPCLIENT_PORT+1000,1);
    sock_del(fd);
}

void TClntService::Run()
{
    // When we get here, the service has been stopped
	//return Client.Status.dwWin32ExitCode;
    string confile=CLNTCONF_FILE;
    string workdir=this->ServiceDir;
	string oldconf=CLNTCONF_FILE+(string)"old";
	string addrfile=CLNTDB_FILE;
    string logFile=workdir+"clntlog.txt";
    char *logChar=new char[logFile.length()+1];
    strcpy(logChar,logFile.c_str());
    logger::Initialize(logChar);
    delete logChar;
    //clog<<logger::logAlert<<"Working dir:"<<ServiceDir;
    logger::setLogname("DHCPv6 Server");
    //char dir[200];
    //_getcwd(dir,200);
    clog<<logger::logInfo<<"Work Dir:"<<workdir<<"  ipv6path:"<<ipv6Path<<endl;
    TDHCPClient client(workdir+confile);
    client.run();
}

TClntService::~TClntService(void)
{
}
