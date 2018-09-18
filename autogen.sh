#! /bin/sh

for filename in deps/libev/ev++.h \
                deps/tinytoml/include/toml/toml.h
do
  if test ! -e "$filename"
  then
    cat <<EOF
**
** $filename is missing.
**
** You need to initialize Git submodules.
**
EOF
    exit 1
  fi
done

if autoheader && autoconf; then
	rm -rf autom4te.cache
	echo "Now run ./configure"
fi
