; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Dibbler - a portable DHCPv6
AppVerName=Dibbler 0.3.0
DefaultDirName={sd}\dibbler
DefaultGroupName=Dibbler
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma
SolidCompression=yes

[Types]
Name: "Full";   Description: "Server, client and docs";
Name: "Server"; Description: "Server only.";
Name: "Client"; Description: "Client only.";

[Components]
Name: "Server"; Description: "DHCPv6 server"; Types: Full Server;
Name: "Client"; Description:"DHCPv6 client"; Types: Full Client;
Name: "Documentation"; Description: "User's Guide"; Types: Full Server Client;

[Files]
Source: "Debug\dibbler-client.exe"; DestDir: "{app}"; Components: Client;
Source: "..\client*.conf"; DestDir: "{app}"; Components: Client;
Source: "client.log"; DestDir: "{app}"; Components: Client;
Source: "Debug\dibbler-server.exe"; DestDir: "{app}"; Components: Server;
Source: "..\server*.conf"; DestDir: "{app}"; Components: Server;
Source: "server.log"; DestDir: "{app}"; Components: Server;
Source: "..\doc\dibbler-user.pdf"; DestDir: "{app}"; Flags: isreadme; Components: Documentation;
Source: "..\CHANGELOG"; DestDir: "{app}"; Components: Documentation;

[Icons]
Name: "{group}\User's Guide"; Filename: "{app}\dibbler-user.pdf"
Name: "{group}\Client Run in the console"; Filename: "{app}\dibbler-client.exe";Parameters: "run -d {app}";
Name: "{group}\Client Install as service"; Filename: "{app}\dibbler-client.exe";Parameters: "install -d {app}";
Name: "{group}\Client Remove service"; Filename: "{app}\dibbler-client.exe";     Parameters: "uninstall -d {app}";
Name: "{group}\Client View log file"; Filename: "notepad.exe"; Parameters: "{app}\client.log"
Name: "{group}\Client Edit config file"; Filename: "notepad.exe"; Parameters: "{app}\client.conf"
Name: "{group}\Server Run in the console"; Filename: "{app}\dibbler-server.exe"; Parameters: "run -d {app}";
Name: "{group}\Server Install as service"; Filename: "{app}\dibbler-server.exe"; Parameters: "install -d {app}";
Name: "{group}\Server Remove service"; Filename: "{app}\dibbler-server.exe";     Parameters: "uninstall -d {app}";
Name: "{group}\Server View log file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\server.log"
Name: "{group}\Server Edit config file"; Filename: "{win}\notepad.exe"; Parameters: "{app}\server.conf"
Name: "{group}\Remove Dibbler"; Filename: {app}\unins000.exe


