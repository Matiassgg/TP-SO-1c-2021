#!/bin/bash

RULE=""
PROJECT="tp-2021-1c-LaMitad-1/"

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

cd $PROJECT
make 

echo -e "\n\nDeploy hecho\n\n"