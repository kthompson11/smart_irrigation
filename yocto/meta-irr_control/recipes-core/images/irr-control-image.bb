SUMMARY = "Irrigation controller image"
LICENSE = "MIT"

IMAGE_FEATURES += "ssh-server-openssh"

inherit core-image

# Configure groups and users
inherit extrausers
EXTRA_USERS_PARAMS = "\
	useradd -p '' irr_user; \
	groupadd spi; \
	usermod -a -G spi irr_user; \
	"

KERNEL_DEVICETREE =+ "overlays/valve-control.dtbo"
