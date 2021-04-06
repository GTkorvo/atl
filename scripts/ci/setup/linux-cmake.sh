FILENAME=$(curl https://cmake.org/files/LatestRelease/cmake-latest-files-v1.json 2>/dev/null | grep 'cmake.*sh' | sed -n 's|.*"\(cmake.*x86_64.sh\).*|\1|p')
VERSION=$(echo ${FILENAME} | sed 's|cmake-\([^\-]*\).*|\1|')

curl -L https://github.com/Kitware/CMake/releases/download/v${VERSION}/${FILENAME} > cmake.sh
chmod +x cmake.sh
./cmake.sh --skip-license --exclude-subdir --prefix=/usr/local
rm -f cmake.sh
