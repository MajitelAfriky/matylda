# Robot Matylda
Tento projekt obsahuje program pro ovládání robota Matyldy na Střední průmyslové škole technické v Jablonci nad Nisou. Robot by měl být k nalezení někde v učebně 429.

## Zapojení
Matylda jede na dvou arduinách, v hlavě je umístěno Uno, které zajišťuje program pro dva 8x8 dispaye. Z boku na hrudi je pak umístěné Mega, na kterém je nahrán celý hlavní program a komunikace se [zvukovým modulem](https://dratek.cz/arduino/4857-hlasovy-modul-s-integrovanym-mp3-prehravacem-dfplayer.html?gad_source=1&gclid=CjwKCAiApaarBhB7EiwAYiMwqtUecnrtwe2t614Tmao8x_ayhX7_WxAD3FYMA9HGZusC4rC5P0ev0hoCVJUQAvD_BwE) na druhé straně hrudi.

Robot je konstruován pro vstupní napětí 6 - 6,5V, vhodný je regulovatelný laboratorní zdroj nebo případně jiný zdroj s dostatečným výkonem (2A).

## Funkce
1. Hlasová reakce na vzádlenost člověka
   * Ultrazvukový senzor změří vzdálenost a po vyhodnocení přehraje příslušnoý .mp3 soubor.

2. Pohybová reakce na vzdálenost člověka
   * Ultrazvukový senzor změří vzdálenost a po vyhodnocení se spustí určitá smyčka s pohybem.

3. Přehrání hudby vesmyčce při neaktivitě
   * Pokuď ultrazvukový senzor dlouho nenaměří vzdálenost, která spoušťí nějakou funkci tak se zapne hudba.
   * Při této funkci dochází k resetování [zvukového modulu](https://dratek.cz/arduino/4857-hlasovy-modul-s-integrovanym-mp3-prehravacem-dfplayer.html?gad_source=1&gclid=CjwKCAiApaarBhB7EiwAYiMwqtUecnrtwe2t614Tmao8x_ayhX7_WxAD3FYMA9HGZusC4rC5P0ev0hoCVJUQAvD) což zamezuje jeho "zaseknutí".

4. DIP přepínač
   * Na hrudi robota se nachází DIP přepínač s deseti kontakty.
     * kontakt 1 slouží ke změně hlasitosti (15/20).
     * kontakt 2 až 7 je volnej k programování.
     * kontakt 8 slouží k vypnutí hudby při neaktivitě.
     * kontakt 9 slouží k vypnutí pohybu robota.
     * kontakt 10 slouží k celkovému ztlumení robota.
    
5. Dvě 8x8 LED patice na hlavě
   * Tato část funguje separátně na druhém Arduinu. Jsou připraveny dvě verze programu [oci.ino](https://github.com/MajitelAfriky/matylda/blob/main/oci.ino) a [oci_v2.ino](https://github.com/MajitelAfriky/matylda/blob/main/oci_v2.ino).
   * Verze [oci_v2.ino](https://github.com/MajitelAfriky/matylda/blob/main/oci_v2.ino) je více zjednodušeá a je připravena na spojení s Arduino Mega přes sériovou komunikaci, ači pak budou moct reagovat na vdálenost.
    
## Odkazy
[Více o Matyldě](https://robot-matylda.webnode.cz/)

![Matylda](https://i.imgur.com/34xNP4A.jpeg)
