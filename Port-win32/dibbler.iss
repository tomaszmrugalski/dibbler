; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Dibbler - a portable DHCPv6
AppVerName=Dibbler 1.0.1 (WinXP/2003/Vista/7/8 port)
OutputBaseFilename=dibbler-1.0.0-win32
OutputDir=..
DefaultDirName={sd}\dibbler
DefaultGroupName=Dibbler
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma
SolidCompression=yes
LicenseFile=..\license
InfoAfterFile=..\RELNOTES
AlwaysShowComponentsList = yes

[Components]
Name: "Server"; Description: "DHCPv6 server"; Types: Full Compact;
Name: "Client"; Description: "DHCPv6 client"; Types: Full Compact;
Name: "Relay";  Description: "DHCPv6 relay"; Types: Full;
Name: "Documentation"; Description: "User's Guide"; Types: Full Compact;
Name: "Tools";  Description: "DHCPv6 requestor and other tools"; Types: Full;

[Files]
Source: "Debug32\bin\dibbler-client.exe"; DestDir: "{app}"; Components: Client;
Source: "..\doc\examples\client*.conf"; DestDir: "{app}\examples"; Components: Client;
Source: "..\doc\examples\client-win32.conf"; DestDir: "{app}"; DestName: "client.conf"; Components: Client;
Source: "client.log"; DestDir: "{app}"; Components: Client;
Source: "Debug32\bin\dibbler-relay.exe"; DestDir: "{app}"; Components: Relay;
Source: "..\doc\examples\relay*.conf"; DestDir: "{app}\examples"; Components: Relay;
Source: "relay.log"; DestDir: "{app}"; Components: Relay;
Source: "Debug32\bin\dibbler-server.exe"; DestDir: "{app}"; Components: Server;
Source: "Debug32\bin\dibbler-requestor.exe"; DestDir: "{app}"; Components: Tools;
Source: "..\doc\examples\server*.conf"; DestDir: "{app}\examples"; Components: Server;
Source: "..\doc\examples\server-win32.conf"; DestDir: "{app}"; DestName: "server.conf"; Components: Server;
Source: "server.log"; DestDir: "{app}"; Components: Server;
Source: "..\doc\dibbler-user.pdf"; DestDir: "{app}"; Components: Documentation;
Source: "..\CHANGELOG"; DestDir: "{app}"; DestName: "CHANGELOG.txt";
Source: "..\RELNOTES"; DestDir: "{app}"; DestName: "RELNOTES.txt"; Flags: isreadme;
Source: "..\AUTHORS"; DestDir: "{app}"; DestName: "AUTHORS.txt";

[Icons]
Name: "{group}\User's Guide"; Filename: "{app}\dibbler-user.pdf"
Name: "{group}\Release notes"; Filename: "notepad.exe"; Parameters: "{app}\RELNOTES.txt"

Name: "{group}\Client Run in the console"; Filename: "{app}\dibbler-client.exe"; WorkingDir: "{app}"; Parameters: " run -d ""{app}"" ";
Name: "{group}\Client Install as service"; Filename: "{app}\dibbler-client.exe"; WorkingDir: "{app}"; Parameters: "install -d ""{app}"" ";
Name: "{group}\Client Remove service"; Filename: "{app}\dibbler-client.exe";     WorkingDir: "{app}"; Parameters: "uninstall";
Name: "{group}\Client View log file"; Filename: "notepad.exe"; WorkingDir: "{app}";  Parameters: "{app}\dibbler-client.log"
Name: "{group}\Client Edit config file"; Filename: "notepad.exe"; WorkingDir: "{app}"; Parameters: "{app}\client.conf"

Name: "{group}\Server Run in the console"; Filename: "{app}\dibbler-server.exe"; WorkingDir: "{app}";  Parameters: "run -d ""{app}"" ";
Name: "{group}\Server Install as service"; Filename: "{app}\dibbler-server.exe"; Parameters: "install -d ""{app}"" ";
Name: "{group}\Server Remove service"; Filename: "{app}\dibbler-server.exe";     Parameters: "uninstall";
Name: "{group}\Server View log file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\dibbler-server.log"
Name: "{group}\Server Edit config file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\server.conf"

Name: "{group}\Relay Run in the console"; Filename: "{app}\dibbler-relay.exe";Parameters: "run -d ""{app}"" ";
Name: "{group}\Relay Install as service"; Filename: "{app}\dibbler-relay.exe";Parameters: "install -d ""{app}"" ";
Name: "{group}\Relay Remove service"; Filename: "{app}\dibbler-relay.exe";     Parameters: "uninstall";
Name: "{group}\Relay View log file"; Filename: "notepad.exe"; Parameters: "{app}\dibbler-relay.log"
Name: "{group}\Relay Edit config file"; Filename: "notepad.exe"; Parameters: "{app}\relay.conf"

Name: "{group}\Remove Dibbler"; Filename: {app}\unins000.exe
