@echo off
REM ============================================================
REM  backup.bat - SAO LUU database student_attendance (NFR-04, NFR-06)
REM  Cach dung: bam doi (double-click) hoac chay: database\backup.bat
REM  Ket qua : database\backups\student_attendance_YYYYMMDD_HHMMSS.sql
REM            va ban sao moi nhat: database\backup.sql
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

set "TOOL=mysqldump"

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

set "OUTDIR=%~dp0backups"
if not exist "%OUTDIR%" mkdir "%OUTDIR%"
for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd_HHmmss"') do set "TS=%%i"
set "OUT=%OUTDIR%\student_attendance_%TS%.sql"

echo Dang sao luu database "%DB_NAME%" tu %DB_HOST%:%DB_PORT% ...
"%BIN%" -h %DB_HOST% -P %DB_PORT% -u %DB_USER% --single-transaction --routines --default-character-set=utf8mb4 --databases %DB_NAME% --result-file="%OUT%"
if errorlevel 1 (
  echo [LOI] Sao luu that bai. Kiem tra MySQL da bat chua va mat khau trong config.ini.
  if exist "%OUT%" del "%OUT%"
  pause
  exit /b 1
)
copy /y "%OUT%" "%~dp0backup.sql" >nul
echo.
echo [OK] Da sao luu xong:
echo      %OUT%
echo      %~dp0backup.sql  (ban moi nhat)
pause
