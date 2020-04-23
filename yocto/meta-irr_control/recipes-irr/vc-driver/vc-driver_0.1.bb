SUMMARY = "Bitbake recipe to build and install the valve-control driver."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit module

SRC_URI = "file://COPYING;subdir=${WORKDIR}/${BPN} \
           file://Makefile;subdir=${WORKDIR}/${BPN} \
           file://vc-driver.c;subdir=${WORKDIR}/${BPN} \
           file://vc-driver.h;subdir=${WORKDIR}/${BPN} \
           file://vc-interface.h;subdir=${WORKDIR}/${BPN} \
          "

S = "${WORKDIR}/${BPN}"

RPROVIDES_${PN} += "kernel-module-valve-control"