dnf install -y epel-release make curl perl

curl -L https://copr.fedorainfracloud.org/coprs/g/git-maint/git/repo/epel-8/group_git-maint-git-epel-8.repo > /etc/yum.repos.d/group_git-maint-git-epel-8.repo

dnf install -y git

source "${SETUP_DIR}/linux-cmake.sh"
