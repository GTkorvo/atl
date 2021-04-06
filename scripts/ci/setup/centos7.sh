yum install -y make git curl perl

curl https://cmake.org/files/LatestRelease/$(curl https://cmake.org/files/LatestRelease/cmake-latest-files-v1.json 2>/dev/null | grep 'cmake.*sh' | sed -n 's|.*"\(cmake.*x86_64.sh\).*|\1|p') > cmake.sh
chmod +x cmake.sh
./cmake.sh --skip-license --exclude-subdir --prefix=/usr/local
rm -f cmake.sh
