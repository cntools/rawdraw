all : index.html

#Path to rawdraw
CFLAGS:=-I..

CLANG?=clang
WASMOPT?=wasm-opt
UGLIFYJS?=uglifyjs

CFLAGS+=-DWASM -nostdlib --target=wasm32 \
		-flto -Oz \
		-Wl,--lto-O3 \
		-Wl,--no-entry \
		-Wl,--allow-undefined \
		-Wl,--import-memory

opt.js : template.js main.wasm
	bash -c 'export BLOB=$$(cat main.wasm | base64 | sed -e "$$ ! {/./s/$$/ \\\\/}" ); envsubst < template.js > opt.js'
	#Comment the below line out if you don't want to uglify the javascript.
	$(UGLIFYJS) opt.js -o opt.js

index.html : template.ht opt.js
	bash -c 'export JAVASCRIPT_DATA=$$(cat opt.js); envsubst < template.ht > $@'

main.wasm: rawdraw.c
	$(CLANG) $(CFLAGS) $^ -o $@
	$(WASMOPT) --asyncify -Oz main.wasm -o main.wasm

clean:
	rm -rf main.wasm opt.js index.html
