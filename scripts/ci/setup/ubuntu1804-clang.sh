SETUP_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "${SETUP_DIR}/ubuntu.sh"
apt install -y clang
