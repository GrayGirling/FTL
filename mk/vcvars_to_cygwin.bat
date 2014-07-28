@rem Visual Studio 2010 uses 'vsvars32' previous C++ studios use 'vcvars32'
@call vsvars32.bat >nul:
@IF %ERRORLEVEL% GTR 0 call vcvars32.bat >nul:
@echo export VC_PATH="%PATH%"
@echo export VC_INCLUDE="%INCLUDE%"
@echo export VC_LIB="%LIB%"
