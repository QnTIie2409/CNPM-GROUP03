@echo off
REM ============================================================
REM  restore.bat - PHUC HOI database student_attendance tu file .sql
REM  Cach dung: database\restore.bat              (dung database\backup.sql)
REM             database\restore.bat duong_dan\file.sql
REM  CANH BAO : se GHI DE toan bo du lieu hien tai cua database.
REM ============================================================
setlocal EnableExtensions
REM ============================================================
REM  Doc cau hinh ket noi tu config.ini (o thu muc goc repo)
REM ============================================================
set "CFG=%~dp0..\config.ini"
set "DB_HOST=127.0.0.1"
set "DB_PORT=3306"
set "DB_NAME=student_attendance"
set "DB_USER=root"
set "DB_PASS="
if exist "%CFG%" (
  for /f "usebackq tokens=1,* delims==" %%A in ("%CFG%") do (
    if /i "%%A"=="host"     set "DB_HOST=%%B"
    if /i "%%A"=="port"     set "DB_PORT=%%B"
    if /i "%%A"=="dbname"   set "DB_NAME=%%B"
    if /i "%%A"=="username" set "DB_USER=%%B"
    if /i "%%A"=="password" set "DB_PASS=%%B"
  )
) else (
  echo [CANH BAO] Khong thay config.ini, dung gia tri mac dinh.
)
REM Truyen mat khau qua bien moi truong de khong bi loi ky tu dac biet
set "MYSQL_PWD=%DB_PASS%"

set "TOOL=mysql"

REM ============================================================
REM  Tim chuong trinh %TOOL%.exe (trong PATH, hoac thu muc cai dat pho bien)
REM ============================================================
set "BIN="
where %TOOL% >nul 2>nul && set "BIN=%TOOL%"
if not defined BIN for %%P in (
  "C:\Program Files\MySQL\MySQL Server 8.0\bin"
  "C:\Program Files\MySQL\MySQL Server 8.4\bin"
  "C:\Program Files\MySQL\MySQL Server 9.0\bin"
  "C:\xampp\mysql\bin"
) do if exist "%%~P\%TOOL%.exe" if not defined BIN set "BIN=%%~P\%TOOL%.exe"
if not defined BIN (
  echo [LOI] Khong tim thay %TOOL%.exe.
  echo       Them thu muc "bin" cua MySQL vao bien moi truong PATH, hoac sua duong dan trong file nay.
  pause
  exit /b 1
)

set "SQLFILE=%~1"
if "%SQLFILE%"=="" set "SQLFILE=%~dp0backup.sql"
if not exist "%SQLFILE%" (
  echo [LOI] Khong thay file sao luu: %SQLFILE%
  echo       Hay chay backup.bat truoc, hoac truyen duong dan file .sql vao.
  pause
  exit /b 1
)

echo File phuc hoi : %SQLFILE%
echo Database      : %DB_NAME% tren %DB_HOST%:%DB_PORT%
echo.
echo CANH BAO: du lieu hien tai cua database se bi GHI DE.
set /p CONFIRM=Go YES de tiep tuc: 
if /i not "%CONFIRM%"=="YES" (
  echo Da huy.
  pause
  exit /b 1
)

for /f %%i in ('powershell -NoProfile -Command "[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()"') do set "T0=%%i"
"%BIN%" -h %DB_HOST% -P %DB_PORT% -u %DB_USER% --default-character-set=utf8mb4 < "%SQLFILE%"
if errorlevel 1 (
  echo [LOI] Phuc hoi that bai. Kiem tra MySQL da bat chua va mat khau trong config.ini.
  pause
  exit /b 1
)
for /f %%i in ('powershell -NoProfile -Command "[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()"') do set "T1=%%i"
set /a SECS=T1-T0
echo.
echo [OK] Phuc hoi xong trong %SECS% giay (yeu cau NFR-04: duoi 15 phut).
pause
