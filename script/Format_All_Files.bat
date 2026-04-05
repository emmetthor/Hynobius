@echo off
echo Formatting tracked files...

for /f "delims=" %%f in ('git ls-files *.cpp *.h *.hpp *.cc') do (
    clang-format -i "%%f"
)

echo Done.
pause