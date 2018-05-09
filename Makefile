SHELL := /bin/bash

CWD=$(shell pwd)
VERSION=$(shell node -pe "require('./library.json').version")

define release
	test -n "$(1)" 
	@git-semver.sh 
	@sed -i 's/version=.*/version=$1/' library.properties
	echo "CURRENT_VERSION=${VERSION}"
	node -e "\
		var j = require('./library.json');\
		j.version = '$(1)';\
		var s = JSON.stringify(j, null, 4) + '\n';\
		require('fs').writeFileSync('./library.json', s);"
	@echo "releasing V=$1"
endef

release:
	@$(call release,$V) 
version:
	echo "CURRENT_VERSION=${VERSION}"
test:
	./pio_build_test.sh