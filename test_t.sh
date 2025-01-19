#!/bin/sh

# Kompilacja programu
gcc -o disk_manager disk_manager.c

# Ścieżka do menedżera dysku
DISK_MANAGER=./disk_manager

# Nazwa i rozmiar wirtualnego dysku
DISK_NAME=test_disk.img
DISK_SIZE=409600 # 400 KB

# Pliki testowe
TEST_FILE_IN=minix_file.txt
TEST_FILE_OUT=ala_ma_kota.txt
LONG_NAME_FILE="plik z długą nazwą.txt"
LONG_FILE="lorem_ipsum.txt"
DOT_FILE=".plik_ukryty"

# Plik, do którego przekierujemy całe wyjście
OUTPUT_FILE="output.txt"
OUTPUT_FILE_COMMANDS="output.txt"


# 1. Tworzenie wirtualnego dysku

echo ""
echo "Tworzenie wirtualnego dysku o rozmiarze 400 KB..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER create $DISK_NAME $DISK_SIZE"
$DISK_MANAGER create $DISK_NAME $DISK_SIZE
echo "ls"
ls
echo "---------------------------------------------------"


# 2. Montowanie wirtualnego dysku

echo ""
echo "Montowanie wirtualnego dysku..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER mount $DISK_NAME"
$DISK_MANAGER mount $DISK_NAME
echo "cat config.txt"
cat config.txt
echo ""
echo "---------------------------------------------------"


# 3. Kopiowanie pliku z systemu Minix na dysk wirtualny

echo ""
echo "Kopiowanie pliku na dysk wirtualny..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER cp -in $TEST_FILE_IN"
$DISK_MANAGER cp -in $TEST_FILE_IN
echo "---------------------------------------------------"

# Informacje o dysku
echo "Informacje o dysku po kopiowaniu pliku:"
echo "---------------------------------------------------"
$DISK_MANAGER info
echo "---------------------------------------------------"


# 4. Obsługa długich nazw ze spacjami

echo ""
echo "Testowanie obsługi długich nazw ze spacjami..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER cp -in \"$LONG_NAME_FILE\""
$DISK_MANAGER cp -in "$LONG_NAME_FILE"
echo "---------------------------------------------------"

# Informacje o dysku
echo "Informacje o dysku po teście długich nazw:"
echo "---------------------------------------------------"
$DISK_MANAGER info
echo "---------------------------------------------------"


# 5. Polecenie ls

echo ""
echo "Testowanie polecenia ls..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER ls"
$DISK_MANAGER ls
echo "$DISK_MANAGER cp -in a.s"
$DISK_MANAGER cp -in a.s
echo "$DISK_MANAGER cp -in b.s"
$DISK_MANAGER cp -in b.s
echo "$DISK_MANAGER ls"
$DISK_MANAGER ls
echo "---------------------------------------------------"


# 6. Kopiowanie pliku z dysku wirtualnego na dysk systemu Minix

echo ""
echo "Kopiowanie pliku z dysku wirtualnego..."
echo ""
echo "---------------------------------------------------"
echo "$DISK_MANAGER cp -in ala_ma_kota.txt"
$DISK_MANAGER cp -in ala_ma_kota.txt
echo "mv ala_ma_kota.txt ala_ma_kota_old.txt"
mv ala_ma_kota.txt ala_ma_kota_old.txt
echo "$DISK_MANAGER cp -out ala_ma_kota.txt"
$DISK_MANAGER cp -out ala_ma_kota.txt
echo "Porównanie plików:"
echo "---------------------------------------------------"
echo "diff ala_ma_kota.txt ala_ma_kota_old.txt"
diff ala_ma_kota.txt ala_ma_kota_old.txt
echo "cat ala_ma_kota.txt"
cat ala_ma_kota.txt
echo "cat ala_ma_kota_old.txt"
cat ala_ma_kota_old.txt
echo "--------------------------------------------------"


# 7. Kopiowanie długiego pliku

echo ""
echo "Testowanie kopiowania długich plików.."
echo ""
echo "--------------------------------------------------"
echo "$DISK_MANAGER cp -in lorem_ipsum.txt"
$DISK_MANAGER cp -in lorem_ipsum.txt
echo "$DISK_MANAGER info"
$DISK_MANAGER info
echo "mv lorem_ipsum.txt lorem_ipsum_old.txt"
mv lorem_ipsum.txt lorem_ipsum_old.txt
echo "$DISK_MANAGER cp -out lorem_ipsum.txt"
$DISK_MANAGER cp -out lorem_ipsum.txt
echo "Porównanie plików:"
echo "--------------------------------------------------"
echo "diff lorem_ipsum.txt lorem_ipsum_old.txt"
diff lorem_ipsum.txt lorem_ipsum_old.txt
echo "--------------------------------------------------"


# 8. Usuwanie pliku z wirtualnego dysku

echo ""
echo "Usuwanie pliku z dysku wirtualnego.."
echo ""
echo "--------------------------------------------------"
echo "$DISK_MANAGER ls"
$DISK_MANAGER ls
echo "$DISK_MANAGER rm $TEST_FILE_IN"
$DISK_MANAGER rm $TEST_FILE_IN
echo "$DISK_MANAGER ls"
$DISK_MANAGER ls
echo "$DISK_MANAGER info"
$DISK_MANAGER info
echo "--------------------------------------------------"


# 9. Obsługa plików kropkowych

echo ""
echo "Testowanie obsługi plików ukrytych.."
echo ""
echo "--------------------------------------------------"
echo "$DISK_MANAGER cp -in \"$DOT_FILE\""
$DISK_MANAGER cp -in "$DOT_FILE"
echo "$DISK_MANAGER ls"
$DISK_MANAGER ls
echo "$DISK_MANAGER ls -a"
$DISK_MANAGER ls -a
echo "--------------------------------------------------"


# 10. Nieciągła alokacja

echo ""
echo "Testowanie nieciągłej alokacji.."
echo ""
echo "--------------------------------------------------"
echo "Usuwanie kilku plików.."
$DISK_MANAGER rm lorem_ipsum.txt
$DISK_MANAGER rm .plik_ukryty
$DISK_MANAGER rm a.s
$DISK_MANAGER rm b.s
$DISK_MANAGER info
echo "--------------------------------------------------"

echo "Kopiowanie pliku na dysk wirtualny.."
echo "$DISK_MANAGER cp -in lorem_ipsum.txt"
$DISK_MANAGER cp -in lorem_ipsum.txt
echo "$DISK_MANAGER info"
$DISK_MANAGER info
echo "--------------------------------------------------"


# 11. Testowanie bloków nazw

echo ""
echo "Testowanie bloków nazw.."
echo ""
echo "--------------------------------------------------"
echo "Usuwanie plików z dysku wirtualnego.."
$DISK_MANAGER rm "plik z długą nazwą.txt"
$DISK_MANAGER rm "ala_ma_kota.txt"
$DISK_MANAGER rm "lorem_ipsum.txt"
$DISK_MANAGER info
echo "--------------------------------------------------"

# Tworzenie 64 plików o długiej nazwie na dysku lokalnym
for i in `seq 1 64`
do
    echo "test bloków nazw $i" > "długa_nazwa_pliku_$i.txt"
done
echo "Wstawianie 64 plików o długiej nazwie i długości 1 bloku.."
for i in `seq 1 64`
do
    $DISK_MANAGER cp -in "długa_nazwa_pliku_$i.txt"
done
echo "$DISK_MANAGER info"
$DISK_MANAGER info
echo "--------------------------------------------------"

# Usunięcie ostatniego pliku
echo "Usunięcie ostatniego pliku.."
$DISK_MANAGER rm "długa_nazwa_pliku_64.txt"
echo "$DISK_MANAGER info"
$DISK_MANAGER info
echo "--------------------------------------------------"


# Usunięcie pliku ze środka
echo "Usunięcie pliku ze środka.."
$DISK_MANAGER rm "długa_nazwa_pliku_32.txt"
echo "$DISK_MANAGER info"
$DISK_MANAGER info


# 12. Usuwanie wirtualnego dysku

echo ""
echo "Usuwanie wirtualnego dysku.."
echo ""
echo "--------------------------------------------------"
echo "$DISK_MANAGER unmount"
$DISK_MANAGER unmount
echo "cat config.txt"
cat config.txt
echo "--------------------------------------------------"

# Usunięcie plików tymczasowych

for i in `seq 1 64`
do
    rm "długa_nazwa_pliku_$i.txt"
done
rm ala_ma_kota_old.txt lorem_ipsum_old.txt
