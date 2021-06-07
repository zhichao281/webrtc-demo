echo off 
set sourcePath=.\src
mkdir include
set resultPath=.\include
xcopy %sourcePath%\*.h %resultPath%\  /s /e /c /y /h /r
pause