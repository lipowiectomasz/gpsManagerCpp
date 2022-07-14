Projekt programu zarządzającego zasilaniem portów GPIO mikrokomputera Raspberry Pi 4, w zależności od dnia tygodnia. Zasilanie uruchamiane jest na 2 wybranych pinach GPIO (domyślnie 17, 27) w dni niebędące niedzielą bądź świętem. Urządzenie współpracuje z modułem GPS GY-NEO6MV2, w celu zapewnienia aktualnej daty i czasu bez konieczności podłączenia malinki do internetu. Synchronizacja czasu oraz ustalenie trybu pracy następuje przy uruchomieniu programu, a następnie raz na dobę o godzinie 4.

Kompilacja: g++ powerManager.cpp managerLibrary.cpp kbhit.cpp getStdout.cpp -o <nazwa> -lwiringPi

Przed uruchomieniem: timedatectl set-ntp 0