echo "Automatic build tests:"

if make assembler > /dev/null; then # Suppress output of make when everything works fine
    echo "[OK] - Assembler build succeeded :o"
else 
    echo "[FAIL] - Assembler build failed :("
fi 

if make project > /dev/null; then # Suppress output of make when everything works fine
    echo "[OK] - Project build succeeded :o"
else 
    echo "[FAIL] - Project build failed :("
fi 