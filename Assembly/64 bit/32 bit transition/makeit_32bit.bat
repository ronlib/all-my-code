"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\ml64" /coff /Fo "F:\Development\Assembly\64 bit\32 bit transition\Main.obj" /c "F:\Development\Assembly\64 bit\32 bit transition\Main.asm"
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\link" /out:"F:\Development\Assembly\64 bit\32 bit transition\32bit.exe" /SUBSYSTEM:CONSOLE /entry:main /base:0x100000 /fixed "F:\Development\Assembly\64 bit\32 bit transition\Main.obj" "C:\WinDDK\7600.16385.1\lib\win7\amd64\kernel32.lib"