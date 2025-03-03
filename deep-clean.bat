@echo off
setlocal enabledelayedexpansion

:: Define folders (one per line)
set "folders="
set folders=windows\packages
set folders=%folders% PB\build-ARM64
set folders=%folders% PB\build-x64
set folders=%folders% windows\PhotobookNet\obj
set folders=%folders% windows\PhotobookNet\bin
set folders=%folders% windows\_build
set folders=%folders% windows\x64
set folders=%folders% windows\arm64

:: Loop through each folder and delete if it exists
for %%F in (%folders%) do (
    if exist "%%F" (
        rd /S /Q "%%F"
        echo Deleted: %%F
    ) else (
        echo Folder not found: %%F
    )
)

echo Done.
pause