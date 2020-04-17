DESCRIPTION = "Adds additional device tree overrides."

do_deploy_append() {
    echo "# Add custom overlays" >> ${DEPLOYDIR}/bcm2835-bootfiles/config.txt
    echo "dtoverlay=test" >> ${DEPLOYDIR}/bcm2835-bootfiles/config.txt
}
