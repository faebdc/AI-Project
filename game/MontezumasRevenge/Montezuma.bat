@echo off
cls
color 0f
echo -------------------------------------------------------------------------------
echo.
echo  Montezuma's Revenge - Atari 8-bit Loader prepared by GamesNostalgia.com
echo.
echo  If you like what we do, support us with a donation:
echo.
echo  ***   http://gamesnostalgia.com/donate   ***
echo.
echo -------------------------------------------------------------------------------
echo.
echo  Movement: Numpad                    FIRE: Right Ctrl
echo.
echo  F1: Menu      F3: Select      F4: Start       F9: Exit
echo.
echo  Joysticks are supported!
echo. 
echo -------------------------------------------------------------------------------
echo.
echo  Thanks to Atari800 for the emulator: https://atari800.github.io/
echo.
echo  Please go to http://gamesnostalgia.com for more retrogames
echo.
echo.

cd atari800
SET GAME=game.atr

CHOICE /C 12 /N /T 10 /D 1 /M "Type [1] for 48k prototype, [2] for 16k Parker Brothers version"
IF ERRORLEVEL 1 SET GAME=game.atr
IF ERRORLEVEL 2 SET GAME=game2.atr
atari800.exe %GAME%