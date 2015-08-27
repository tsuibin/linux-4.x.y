cd /tmp/nanopi-modules/lib/
find . -name \*.ko | xargs arm-linux-strip --strip-unneeded
tar czvf kernel-modules.tgz modules/
