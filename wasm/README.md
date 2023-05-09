# rawdrawwasm

rawdrawwasm

## Prerequisites

```
sudo apt-get install npm binaryen clang llvm lld
sudo npm install terser -g
```

You may need to add llvm bins to your path, i.e.

```
export PATH=$PATH:/usr/lib/llvm-10/bin
```

Note also: clang. llvm, lld-12 has also been tested and works.

Depending on your llvm version.


## Old Notes

My stab at rawdraw on wasm.  This is based on:
 https://github.com/zNoctum/wasm-tool
and has inspiration from:
 https://github.com/cnlohr/wasm_integrated

Lots of stuff todo, hopefully this will get rolled into main rawdraw.

TODO notes for now:
 * hook up button presses, and mouse input and destroy
 * hook up sleep in a more meaningful way maybe?
 * Make printf work.
 * Make libc work.
 * Figure out how to #include math.h
 * Come up with guide on how to use this.

Check it out, live: https://cnlohr.github.io/rawdrawwasm/


I wanted to compare the difference beteen an asynchronous and synchronous style.  Also, measure the overhead introudced by adding `--asyncify`.  This is using the canvas 2d methods. Those are being abandoned.


```
//TEST ON CHROME:
//NOT RASTERIZER
// Async drive: ~7.2ms
// Sync drive: ~6.9ms
//sync w/o asyncify: 7.0ms.
//
//RASTERIZER
//Async: 7.5ms
//Sync: 3.9ms
//Sync w/o asyncify: 3.6ms

//TEST ON FIREFOX:
//Async: 17.3ms
//sync: 13.7ms
//Sync w/o asyncify: 13.3ms
//
//FF without including swap.
//Async: 11.5
//Sync:  11.5ms
//Sync w/o asyncify: 11.2ms
```


This was something I found convenient for a while...
```

	function wasmloaded(wa)
	{
		instance = wa;
		wasmExports = instance.exports;
		instance.exports.main();
	}

	//If at all possible, we should attempt to load in-thread
	//This will make the load not flash.
	if( blob.length < 4096 )
	{
		let mod = new WebAssembly.Module(array);
		var wa = new WebAssembly.Instance( mod, imports );
 		wasmloaded( wa );
	}
	else
	{
		WebAssembly.instantiate(array, imports).then( function(wa) { wasmloaded(wa.instance); } );
	}
```

