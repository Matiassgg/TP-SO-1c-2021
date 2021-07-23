#!/bin/bash

RULE=""
cd ..

echo -e "\n\nInstalando so-commons-library...\n\n"
git clone "https://github.com/sisoputnfrba/so-commons-library.git"
cd so-commons-library
sudo make install
cd ..

echo -e "\n\nInstalando ncurses...\n\n"
sudo apt-get install libncurses5-dev

echo -e "\n\nInstalando so-nivel-gui-library..\n\n"
git clone "https://github.com/sisoputnfrba/so-nivel-gui-library/"
cd so-nivel-gui-library
sudo make install
cd ..

PROJECTS=(utils Mi-RAM-HQ discordiador i-Mongo-Store)

echo -e "\n\nCompilando modulos...\n\n"

for i in "${PROJECTS[@]}"
do
  echo -e "\n\nBuilding ${i}\n\n"
  cd $i
  make $RULE
  cd $CWD
done

echo -e "\n\nDeploy hecho\n\n"