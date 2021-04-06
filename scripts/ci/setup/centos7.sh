yum install -y epel-release make curl perl

curl -L https://copr.fedorainfracloud.org/coprs/g/git-maint/git/repo/epel-7/group_git-maint-git-epel-7.repo > /etc/yum.repos.d/group_git-maint-git-epel-7.repo

yum install -y git

source "${SETUP_DIR}/linux-cmake.sh"
