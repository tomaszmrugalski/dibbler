; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Dibbler - a portable DHCPv6
AppVerName=Dibbler 0.7.2 (WinNT/2000 port)
OutputBaseFilename=dibbler-0.7.2-winnt2k
OutputDir=..
DefaultDirName={sd}\dibbler
DefaultGroupName=Dibbler
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma
SolidCompression=yes
LicenseFile=..\license
InfoAfterFile=..\RELNOTES

[Components]
Name: "Server"; Description: "DHCPv6 server"; Types: Full Compact;
Name: "Client"; Description: "DHCPv6 client"; Types: Full Compact;
Name: "Tools";  Description: "Relay, requestor (for leasequery) and other tools.";  Types: Full;
Name: "Documentation"; Description: "User's Guide"; Types: Full Compact;

[Files]
Source: "dibbler-client.exe"; DestDir: "{app}"; Components: Client;
Source: "..\client*.conf"; DestDir: "{app}"; Components: Client;
Source: "client.log"; DestDir: "{app}"; Components: Client;
Source: "dibbler-relay.exe"; DestDir: "{app}"; Components: Tools;
Source: "..\relay*.conf"; DestDir: "{app}"; Components: Tools;
Source: "relay.log"; DestDir: "{app}"; Components: Tools;
Source: "dibbler-server.exe"; DestDir: "{app}"; Components: Server;
Source: "..\server*.conf"; DestDir: "{app}"; Components: Server;
Source: "server.log"; DestDir: "{app}"; Components: Server;
Source: "..\doc\dibbler-user.pdf"; DestDir: "{app}"; Components: Documentation;
Source: "..\CHANGELOG"; DestDir: "{app}"; Components: Documentation;
Source: "..\RELNOTES"; DestDir: "{app}"; Components: Documentation;

[Icons]
Name: "{group}\User's Guide"; Filename: "{app}\dibbler-user.pdf"
Name: "{group}\Release notes"; Filename: "notepad.exe"; Parameters: "{app}\RELNOTES"

Name: "{group}\Client Run in the console"; Filename: "{app}\dibbler-client.exe";Parameters: "run -d {app}";
Name: "{group}\Client Install as service"; Filename: "{app}\dibbler-client.exe";Parameters: "install -d {app}";
Name: "{group}\Client Remove service"; Filename: "{app}\dibbler-client.exe";     Parameters: "uninstall";
Name: "{group}\Client View log file"; Filename: "notepad.exe"; Parameters: "{app}\client.log"
Name: "{group}\Client Edit config file"; Filename: "notepad.exe"; Parameters: "{app}\client.conf"

Name: "{group}\Server Run in the console"; Filename: "{app}\dibbler-server.exe"; Parameters: "run -d {app}";
Name: "{group}\Server Install as service"; Filename: "{app}\dibbler-server.exe"; Parameters: "install -d {app}";
Name: "{group}\Server Remove service"; Filename: "{app}\dibbler-server.exe";     Parameters: "uninstall";
Name: "{group}\Server View log file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\server.log"
Name: "{group}\Server Edit config file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\server.conf"

Name: "{group}\Relay Run in the console"; Filename: "{app}\dibbler-run.exe";Parameters: "run -d {app}";
Name: "{group}\Relay Install as service"; Filename: "{app}\dibbler-relay.exe";Parameters: "install -d {app}";
Name: "{group}\Relay Remove service"; Filename: "{app}\dibbler-relay.exe";     Parameters: "uninstall";
Name: "{group}\Relay View log file"; Filename: "notepad.exe"; Parameters: "{app}\relay.log"
Name: "{group}\Relay Edit config file"; Filename: "notepad.exe"; Parameters: "{app}\relay.conf"

Name: "{group}\Remove Dibbler"; Filename: {app}\unins000.exe


