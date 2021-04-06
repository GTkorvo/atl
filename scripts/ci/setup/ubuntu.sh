export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y make git curl perl software-properties-common

apt-get update
apt-get install git -y

source "${SETUP_DIR}/linux-cmake.sh"
