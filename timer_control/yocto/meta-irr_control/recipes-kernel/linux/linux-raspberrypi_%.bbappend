SRC_URI += "file://test-overlay.dts;subdir=git/arch/arm/boot/dts/overlays"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

KERNEL_DEVICETREE += "overlays/test.dtbo"
