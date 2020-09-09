# Smart Irrigation

Smart Irrigation is a project aimed at creating a smart irrigation timer. The timer will be programmable through a web interface and will be able to query a weather API in order to adjust its watering schedule.

## Components

- vc-firm - contains the firmware for the microcontroller.
- yocto - yocto project layer and configuration files for building the linux distribution
- vc-driver - Linux driver for the vc-firm system

## Requirements

- STM32F0 Discovery board
- Raspberry Pi Zero W
- open source STLink tools
- GNU ARM embedded toolchain

## Installing Firmware

1. Download and install the open source STLink tools.
2. Download and install the GNU ARM embedded toolchain.
3. Navigate to `vc-firm/`
4. Build the firmware with `make`
5. Flash the firmware to the STM32F0 Discovery board with `make flash`

## Building the Embedded Linux Distribution

1. Navigate to `yocto/meta-irr_control/recipes-connectivity/wpa-supplicant/wpa-supplicant`
2. `cp wpa_supplicant.conf-sane.example wpa_supplicant.conf-sane`
3. Edit `wpa_supplicant.conf-sane` and fill out the wireless network information.
4. Navigate to the project root directory, download the yocto project reference distribution Poky and extract to directory `poky/`.
5. Navigate to the poky directory and checkout the `zeus` branch by entering `git checkout zeus`
6. Create a symbolic link from `meta-irrcontrol` to the current directory: `ln -s path/to/yocto/meta-irrcontrol meta-irrcontrol`
7. `source oe-init-build-environment`
8. Remove the conf directory: `rm -rf conf`
9. Create a symbolic link to the provided `conf` directory: `ln -s path/to/yocto/conf conf`
10. Compile the dev image: `bitbake irr-control-dev-image`
11. Navigate to `poky/build/tmp/deploy/images/raspberrypi0-wifi`
12. Copy the image `irr-control-dev-image-raspberrypi0-wifi.rpi-sdimg` to an SD card using `dd`
13. Insert the card into a Raspberry Pi Zero W, boot, and login using SSH.