; LDM Download Manager - Windows Installer
; NSIS Script

!define APPNAME "LDM Download Manager"
!define COMPANYNAME "aerab243"
!define DESCRIPTION "A powerful download manager with browser integration"
!define VERSIONMAJOR 1
!define VERSIONMINOR 0
!define VERSIONBUILD 0
!define HELPURL "https://github.com/aerab243/ldm"
!define UPDATEURL "https://github.com/aerab243/ldm/releases"
!define ABOUTURL "https://github.com/aerab243/ldm"
!define INSTALLSIZE 50000

RequestExecutionLevel admin
InstallDir "$PROGRAMFILES\${APPNAME}"
Name "${APPNAME}"
outFile "${APPNAME}-installer.exe"

!include LogicLib.nsh
!include nsDialogs.nsh
!include WinMessages.nsh

page directory
page instfiles

!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
    messageBox mb_iconstop "Administrator rights required!"
    setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    quit
${EndIf}
!macroend

function .onInit
    setShellVarContext all
    !insertmacro VerifyUserIsAdmin
functionEnd

section "install"
    # Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
    setOutPath $INSTDIR

    # Main application files
    file /r "..\..\desktop\build\release\*"  ; Assuming build output is in desktop/build/release

    # Browser extension manifests
    file "..\..\browser-extension\manifest.json"
    file "..\..\browser-extension\manifest-firefox.json"
    file "..\..\browser-extension\manifest-safari.json"

    # Native messaging manifests
    file "..\..\native-messaging\host\manifest\chrome-manifest.json"
    file "..\..\native-messaging\host\manifest\firefox-manifest.json"
    file "..\..\native-messaging\host\manifest\edge-manifest.json"

    # Documentation
    file /r "..\..\docs\*"

    # Registry information for add/remove programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\ldm.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "${COMPANYNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink" "${HELPURL}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "${UPDATEURL}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "${ABOUTURL}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSIONMINOR}
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" ${INSTALLSIZE}

    # Start Menu
    createShortCut "$SMPROGRAMS\${APPNAME}.lnk" "$INSTDIR\ldm.exe" "" "$INSTDIR\ldm.exe"

    # Desktop shortcut
    createShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\ldm.exe" "" "$INSTDIR\ldm.exe"

    # Registry information for file associations
    WriteRegStr HKCR ".ldm" "" "LDM.Download"
    WriteRegStr HKCR "LDM.Download" "" "LDM Download File"
    WriteRegStr HKCR "LDM.Download\DefaultIcon" "" "$INSTDIR\ldm.exe,0"
    WriteRegStr HKCR "LDM.Download\shell\open\command" "" "$\"$INSTDIR\ldm.exe$\" $\"%1$\""

    # Create uninstaller
    writeUninstaller "$INSTDIR\uninstall.exe"

sectionEnd

section "uninstall"
    # Stop the application if running
    ExecWait '"$INSTDIR\ldm.exe" --uninstall'

    # Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
    DeleteRegKey HKCR ".ldm"
    DeleteRegKey HKCR "LDM.Download"

    # Remove files
    delete "$INSTDIR\uninstall.exe"
    rmDir /r "$INSTDIR"

    # Remove shortcuts
    delete "$SMPROGRAMS\${APPNAME}.lnk"
    delete "$DESKTOP\${APPNAME}.lnk"

sectionEnd
