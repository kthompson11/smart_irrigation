SRC_URI += "file://valve-control-overlay.dts;subdir=git/arch/arm/boot/dts/overlays"
SRC_URI += "file://bcm2708-rpi-zero-w.dts.patch"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

KERNEL_DEVICETREE += "overlays/valve-control.dtbo"
