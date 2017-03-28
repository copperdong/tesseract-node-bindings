VERSION := $(shell node -e "console.log(require('./package.json').version)")

test:
	npm test
.PHONY: test


release:
	@echo "Tagging release $(VERSION)"
	@git tag -m "$(VERSION)" v$(VERSION)

	@echo "Pushing tags to GitHub"
	@git push --tags

	@echo "Publishing to NPM"
	@npm publish
.PHONY: release



# Below build, coverage and clean tasks were partly lifted from https://github.com/geo-data/node-mapserv/blob/e99b23a44d910d444f5a45d144859758f820e1d1/Makefile
# @author danschultzer

# The location of the `istanbul` JS code coverage framework. Try and get a
# globally installed version, falling back to a local install.
ISTANBUL := $(shell which istanbul)
ifeq ($(ISTANBUL),)
	ISTANBUL = ./node_modules/.bin/istanbul/lib/cli.js
endif

# The location of the `node-pre-gyp` module builder. Try and get a globally
# installed version, falling back to a local install.
NODE_GYP = $(shell which node-gyp)
ifeq ($(NODE_GYP),)
	NODE_GYP = ./node_modules/.bin/node-gyp
endif

NODE := $(shell which node)
test_deps = build \
./test/*.js \
$(NODE)

build: build/Debug/tesseract.node
build/Debug/tesseract.node:
	$(NODE_GYP) --verbose --debug rebuild

# Perform the code coverage
cover: coverage/index.html
coverage/index.html: coverage/node-tesseract.info
	genhtml --output-directory coverage coverage/node-tesseract.info
	@printf "%b%s%b\n" "\033[0;32m" "Point your browser at \`coverage/index.html\`" "\033[m\017"
coverage/node-tesseract.info: coverage/bindings.info
	lcov --test-name node-tesseract \
	--add-tracefile coverage/lcov.info \
	--add-tracefile coverage/bindings.info \
	--output-file coverage/node-tesseract.info
coverage/bindings.info: coverage/addon.info
	lcov --extract coverage/addon.info '*src/*' --output-file coverage/bindings.info
coverage/addon.info: coverage/lcov.info
	lcov --capture --base-directory build/ --directory . --output-file coverage/addon.info
# This generates the JS lcov info as well as gcov `*.gcda` files:
coverage/lcov.info: $(test_deps)
	$(NODE) --nouse_idle_notification --expose-gc \
	$(ISTANBUL) cover --report lcovonly -- test/unit.js

# Clean up any generated files
clean:
	$(NODE_GYP) clean
	rm -rf coverage
	rm -rf build
