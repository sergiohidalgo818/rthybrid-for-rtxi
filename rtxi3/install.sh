#!/bin/sh

# Step 1: Build and install
cmake -B build
cmake --build build/
sudo cmake --install ./build

# Step 2: Inform where the plugins are installed
PLUGIN_SRC_DIR="/usr/local/bin/rtxi_modules"
PLUGIN_DEST_DIR="/usr/bin/rtxi_modules"

echo "Plugins have been installed to: $PLUGIN_SRC_DIR"

# Step 3: Ask user for confirmation before copying
echo -n "Do you want to copy the plugins to $PLUGIN_DEST_DIR? [y/N]: "
read user_input

if [ "$user_input" = "y" ] || [ "$user_input" = "Y" ]; then
  echo "Copying plugins to $PLUGIN_DEST_DIR..."
  sudo mkdir -p "$PLUGIN_DEST_DIR"
  sudo cp "$PLUGIN_SRC_DIR"/librthybrid_* "$PLUGIN_DEST_DIR"/
  echo "Plugins successfully copied."
else
  echo "Skipping plugin copy."
fi
