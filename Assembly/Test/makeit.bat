@echo off

    if exist "MyFirstAssembly.obj" del "MyFirstAssembly.obj"
    if exist "MyFirstAssembly.exe" del "MyFirstAssembly.exe"

    c:\masm32\bin\ml /coff /Fo "F:\Development\Assembly\Test\MyFirstAssembly.obj" /c "F:\Development\Assembly\Test\MyFirstAssembly.asm"
    if errorlevel 1 goto errasm

    c:\masm32\bin\Link /SUBSYSTEM:CONSOLE "F:\Development\Assembly\Test\MyFirstAssembly.obj"
    if errorlevel 1 goto errlink
    dir "MyFirstAssembly.*"
    goto TheEnd

  :errlink
    echo _
    echo Link error
    goto TheEnd

  :errasm
    echo _
    echo Assembly Error
    goto TheEnd
    
  :TheEnd

pause
