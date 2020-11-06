yum install -y epel-release https://repo.ius.io/ius-release-el7.rpm
yum makecache -y
yum remove -y git
yum install -y make cmake3 git224
