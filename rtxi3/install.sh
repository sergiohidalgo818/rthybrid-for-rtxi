cmake -B build
cmake --build build/
sudo cmake --install ./build

sudo cp /usr/local/bin/rtxi_modules/* /usr/bin/rtxi_modules/.
