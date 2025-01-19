#!/bin/sh

rm output.txt 2> /dev/null
rm output2.txt 2> /dev/null
rm output3.txt 2> /dev/null

# Dodaj uprawnienia
chmod +x test_f.sh

# Uruchom skrypt i przekieruj wyjście do pliku
./test_f.sh

