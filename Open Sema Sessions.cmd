@echo off
cd /d "%~dp0"
if not exist fkwu.exe (
  gcc -O2 -o fkwu.exe runtime\fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
  if errorlevel 1 (
    pause
    exit /b 1
  )
)
set "SESSION_SOURCE=%TEMP%\sema-sessions.fk"
copy /b form\form-stdlib\core.fk+form\form-stdlib\session-evidence.fk+form\form-stdlib\session-app.fk+form\form-stdlib\session-app-live.fk+form\form-stdlib\session-app-run.fk "%SESSION_SOURCE%" >nul
start "Sema Sessions" /b fkwu.exe "%SESSION_SOURCE%"
timeout /t 1 /nobreak >nul
start "" http://127.0.0.1:8765/
