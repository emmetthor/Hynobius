@echo off
echo Formatting tracked files...

for /f "delims=" %%f in ('git ls-files ^| findstr /r "\.cpp$ \.cc$ \.h$ \.hpp$"') do (
    echo Formatting %%f
    clang-format -i "%%f"
)

echo Done.
pause