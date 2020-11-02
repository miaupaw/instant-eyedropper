#define appname         'Instant Eyedropper'
#define exename         'InstantEyedropper'
#define appversion      GetFileVersion('vsproject/output/InstantEyedropper.exe')
#define productversion  GetStringFileInfo('vsproject/output/InstantEyedropper.exe', "ProductVersion")

[Setup]
AppName              = {#appname}
AppVerName           = {#appname} {#appversion}
AppPublisher         = Spicebrains
AppPublisherURL      = http://www.instant-eyedropper.com/
DefaultDirName       = {pf}\InstantEyedropper
DefaultGroupName     = Instant Eyedropper
WindowVisible        = no
Compression          = lzma/ultra
SolidCompression     = true
OutputDir            = /
OutputBaseFilename   = instant-eyedropper-{#productversion}
LicenseFile          = {#SourcePath}vsproject\output\license.txt
UninstallDisplayIcon = {app}\InstantEyedropper.exe

DisableWelcomePage      = no
DisableDirPage          = no
DisableProgramGroupPage = yes
DisableReadyPage        = yes

[Languages]
Name: en; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon;      Description: "Create a &Desktop icon";        GroupDescription: "Additional icons:";
Name: quicklaunchicon;  Description: "Create a &Quick Launch icon";   GroupDescription: "Additional icons:";
Name: autolaunch;       Description: "Launch on Windows startup";     GroupDescription: "Options:";

[Registry]
Root: HKCU; SubKey: SOFTWARE\SpiceBrains; ValueType: string; ValueName: AppPath; ValueData: {app}
Root: HKCU; SubKey: SOFTWARE\SpiceBrains; ValueType: string; ValueName: exePath; ValueData: {app}\InstantEyedropper.exe

[Files]
Source: "{#SourcePath}vsproject\output\InstantEyedropper.exe";          DestDir: "{app}"; Flags: ignoreversion comparetimestamp
Source: "{#SourcePath}vsproject\output\license.txt";                    DestDir: "{app}"
Source: "{#SourcePath}vsproject\output\readme.txt";                     DestDir: "{app}"
Source: "{#SourcePath}vsproject\output\website.url";                    DestDir: "{app}"; AfterInstall: SetAutoLaunch()


[Icons]
Name: {group}\Instant Eyedropper;            Filename: {app}\InstantEyedropper.exe;  WorkingDir: {app}; 
Name: {group}\Instant Eyedropper Readme;     Filename: {app}\readme.txt;             WorkingDir: {app}; 
Name: {group}\Visit Instant Eyedropper Home; Filename: {app}\website.url;            WorkingDir: {app}; 
Name: {group}\uninstall;                     Filename: {uninstallexe};                                  
Name: {userdesktop}\Instant Eyedropper;      Filename: {app}\InstantEyedropper.exe;  WorkingDir: {app}; MinVersion: 4,4; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Instant Eyedropper; Filename: {app}\InstantEyedropper.exe; WorkingDir: {app}; Tasks: quicklaunchicon;


[Code]
procedure InitializeWizard();
begin
  // меняет страницу лицензии
  WizardForm.LicenseAcceptedRadio.Checked := true;
  WizardForm.LicenseAcceptedRadio.Hide;
  WizardForm.LicenseNotAcceptedRadio.Hide;
  WizardForm.LicenseMemo.Height := ScaleY(190);
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  // меняет текст на кнопках в зависимости от текущей страницы 
  if      CurPageID = wpLicense     then
    WizardForm.NextButton.Caption := 'I Agree'

  else if CurPageID = wpSelectTasks then
    WizardForm.NextButton.Caption := SetupMessage(msgButtonInstall)

  else if CurPageID = wpFinished    then
    WizardForm.NextButton.Caption := SetupMessage(msgButtonFinish)

  else
    WizardForm.NextButton.Caption := SetupMessage(msgButtonNext);

end; 

procedure SetAutoLaunch();
begin
if IsTaskSelected('autolaunch') then
    begin
      RegWriteStringValue(HKEY_CURRENT_USER,
                          'Software\Microsoft\Windows\CurrentVersion\Run\',
                          'instanteyedropper',
                          '"' + ExpandConstant('{app}') + '\InstantEyedropper.exe"');
    end;
end;

[Run]
Filename: "{app}\InstantEyedropper.exe"; Description: "Launch Instant Eyedropper"; Flags: postinstall nowait skipifsilent

[Code]
const WM_CLOSE = $0010;
function InitializeSetup(): Boolean;
  var Wnd: HWND;
begin
  Wnd := FindWindowByClassName('ed_winlass');
  if Wnd <> 0 then SendMessage(Wnd, WM_CLOSE, 0, 0);
  Result := True;
end;

function InitializeUninstall(): Boolean;
//  var ErrorCode: Integer;
  var Wnd: HWND;
begin
  Wnd := FindWindowByClassName('ed_winlass');
  if Wnd <> 0 then SendMessage(Wnd, WM_CLOSE, 0, 0);
  result := True;
end;







