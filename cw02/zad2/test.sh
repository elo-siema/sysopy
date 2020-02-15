#rm -rf ./wyniki.txt


echo "Sortowanie:" >> wyniki.txt
for records in 2000 2500
do
    for size in 4 512 4096 8192
    do
        
        ./main.o generate dane $records $size
        cp dane dane2
        echo "Sortowanie, rozmiar rekordu: $size, rekordów: $records"
        { /usr/bin/time -f "lib: %es real, %Us user, %Ss sys - size: $size, records: $records" ./main.o sort dane $records $size lib ; } >> wyniki.txt 2>&1
        { /usr/bin/time -f "sys: %es real, %Us user, %Ss sys - size: $size, records: $records" ./main.o sort dane2 $records $size sys ; } >> wyniki.txt 2>&1
        rm -rf dane dane2
    done
done


echo "Kopiowanie:" >> wyniki.txt
for records in 100000 200000
do
    for size in 4 512 4096 8192
    do
        
        ./main.o generate dane $records $size
        echo "Kopiowanie, rozmiar rekordu: $size, rekordów: $records"
        { /usr/bin/time -f "lib: %es real, %Us user, %Ss sys - size: $size, records: $records" ./main.o copy dane danelib $records $size lib ; } >> wyniki.txt 2>&1
        { /usr/bin/time -f "sys: %es real, %Us user, %Ss sys - size: $size, records: $records" ./main.o copy dane danesys $records $size sys ; } >> wyniki.txt 2>&1
        rm -rf dane danelib danesys
    done
done