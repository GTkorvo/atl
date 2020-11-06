SETUP_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "${SETUP_DIR}/centos7.sh"
yum install -y clang glibc-devel gcc
