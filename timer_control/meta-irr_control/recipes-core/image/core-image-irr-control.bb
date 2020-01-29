SUMMARY = "Minimal system to support the irrigation controller."
LICENSE = "MIT"

IMAGE_FEATURES = "ssh-server-openssh"

IMAGE_INSTALL = "\
    packagegroup-core-boot \
    packagegroup-base-extended \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

inherit core-image
