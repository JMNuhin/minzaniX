@echo off
REM Configure with AirSim include/libs and Eigen, then build Release
cmake -S D:\minzaniX-main\minzaniX-main -B D:\minzaniX-main\minzaniX-main\build_airsim -DAIRSIM_AVAILABLE=ON -DAIRSIM_INCLUDE_DIR="D:\airsim\AirSim\AirLib\include" -DAIRSIM_LIB_DIR="D:\airsim\AirSim\AirLib\deps\rpclib\lib\x64\Release;D:\airsim\AirSim\AirLib\lib\x64\Release;D:\airsim\AirSim\AirLib\deps\MavLinkCom\lib\x64\Release" -DEIGEN3_INCLUDE_DIR="D:\airsim\AirSim\AirLib\deps\eigen3" -DAIRSIM_LIBS="rpc;AirLib;MavLinkCom" -G "Visual Studio 17 2022"
if ERRORLEVEL 1 (
echo CMake configuration failed & exit /b 1
)
cmake --build D:\minzaniX-main\minzaniX-main\build_airsim --config Release
if ERRORLEVEL 1 (
echo Build failed & exit /b 2
)
REM If executable exists, run it in background and capture output for 6 seconds









exit /b 0)  echo Executable not found: %EXE%) else (  echo Done. Output saved to build_airsim\fc_host_out.txt and fc_host_err.txt  taskkill /im fc_host.exe /f >nul 2>&1 || echo fc_host.exe already exited  timeout /t 6 /nobreak >nul  start "fc_host_run" /B cmd /c ""%EXE%" > D:\minzaniX-main\minzaniX-main\build_airsim\fc_host_out.txt 2> D:\minzaniX-main\minzaniX-main\build_airsim\fc_host_err.txt" nset EXE=D:\minzaniX-main\minzaniX-main\build_airsim\Release\fc_host.exe
nif exist "%EXE%" (
n  echo Running fc_host.exe for 6 seconds...