SHELL := /bin/bash

CWD=$(shell pwd)
VERSION=$(shell node -pe "require('./library.json').version")

define release
	test -n "$(1)" 
	git-semver.sh 
	@sed -i 's/version=.*/version=$1/' library.properties
	echo "CURRENT_VERSION=${VERSION}" 
	node -e "\
		var j = require('./library.json');\
		j.version = '$(1)';\
		var s = JSON.stringify(j, null, 4) + '\n';\
		require('fs').writeFileSync('./library.json', s);"
	@git commit -am "Update meta version code for V$1"
	echo "/* Generated file, do not edit. */" > src/version.h
	echo "#define APP_VERSION \"$1\"" >> src/version.h
	echo "#define APP_VERSION_DATE \"`git log -n 1 --format=%ai`\"" >> src/version.h
	@git commit -am "Update library version for V$1"
	git tag -a "v$(1)" -m "release $(1)"
	git push --tags origin HEAD:master
	@echo "releasing V=$1"
endef

release:
	@$(call release,$V) 
version:
	echo "CURRENT_VERSION=${VERSION}"
test:
	./pio_build_test.sh