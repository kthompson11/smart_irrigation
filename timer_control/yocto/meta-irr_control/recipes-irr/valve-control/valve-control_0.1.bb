SUMMARY = "Bitbake recipe to build and install the valve-control driver."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=54575e81a786e9aa7d98337ec2e1ebb0"

inherit module

SRC_URI = "file://COPYING;subdir=${WORKDIR}/${BPN} \
           file://Makefile;subdir=${WORKDIR}/${BPN} \
           file://valve-control.c;subdir=${WORKDIR}/${BPN} \
          "

S = "${WORKDIR}/${BPN}"

RPROVIDES_${PN} += "kernel-module-valve-control"