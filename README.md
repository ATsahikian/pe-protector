# pe-protector
pe-protector protects executable files

cmake -S ../pe-protector/ -B .  -DVCPKG_TARGET_TRIPLET=x86-windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"

cmake -S ../pe-protector/ -B . -DVCPKG_APPLOCAL_DEPS=OFF  -DVCPKG_TARGET_TRIPLET=x86-windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"

# For Developers
- configure `cmake -S ../src/ -B . -AWin32`

![Your Repository's Stats](https://github-readme-stats.vercel.app/api?username=atsahikian&show_icons=true)

![Your Repository's Stats](https://github-readme-stats.vercel.app/api/top-langs/?username=atsahikian&theme=blue-green)

![Viewer Status](https://komarev.com/ghpvc/?username=atsahikian)

![Build Status](https://travis-ci.com/ATsahikian/pe-protector.svg?branch=master)
