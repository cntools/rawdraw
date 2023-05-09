//Portions of code from zNoctum and redline2466

//Global memory for application.
let memory = new WebAssembly.Memory({initial:200});
let HEAP8 = new Int8Array(memory.buffer);
let HEAPU8 = new Uint8Array(memory.buffer);
let HEAP16 = new Int16Array(memory.buffer);
let HEAPU16 = new Uint16Array(memory.buffer);
let HEAP32 = new Uint32Array(memory.buffer);
let HEAPU32 = new Uint32Array(memory.buffer);
let HEAPF32 = new Float32Array(memory.buffer);
let HEAPF64 = new Float64Array(memory.buffer);

let toUtf8Decoder = new TextDecoder( "utf-8" );
function toUTF8(ptr) {
	let len = 0|0; ptr |= 0;
	for( let i = ptr; HEAPU8[i] != 0; i++) len++;
	return toUtf8Decoder.decode(HEAPU8.subarray(ptr, ptr+len));
}

let wasmExports;
const DATA_ADDR = 16|0; // Where the unwind/rewind data structure will live.
let rendering = false;
let fullscreen = false;

//Configure WebGL Stuff (allow to be part of global context)
let canvas = document.getElementById('canvas');
let wgl = canvas.getContext('webgl');
if( !wgl )
{
	//Janky - on Firefox 83, with NVIDIA GPU, you need to ask twice.
	wgl = canvas.getContext('webgl');
}
let wglShader = null; //Standard flat color shader
let wglABV = null;    //Array buffer for vertices
let wglABC = null;    //Array buffer for colors.
let wglUXFRM = null;  //Uniform location for transform on solid colors


//Utility stuff for WebGL sahder creation.
function wgl_makeShader( vertText, fragText )
{
	let vert = wgl.createShader(wgl.VERTEX_SHADER);
	wgl.shaderSource(vert, vertText );
	wgl.compileShader(vert);
	if (!wgl.getShaderParameter(vert, wgl.COMPILE_STATUS)) {
			alert(wgl.getShaderInfoLog(vert));
	}

	let frag = wgl.createShader(wgl.FRAGMENT_SHADER);
	wgl.shaderSource(frag, fragText );
	wgl.compileShader(frag);
	if (!wgl.getShaderParameter(frag, wgl.COMPILE_STATUS)) {
			alert(wgl.getShaderInfoLog(frag));
	}
	let ret = wgl.createProgram();
	wgl.attachShader(ret, frag);
	wgl.attachShader(ret, vert);
	wgl.linkProgram(ret);
	wgl.bindAttribLocation( ret, 0, "a0" );
	wgl.bindAttribLocation( ret, 1, "a1" );
	return ret;
}

{
	//We load two shaders, one is a solid-color shader, for most rawdraw objects.
	wglShader = wgl_makeShader( 
		"uniform vec4 xfrm; attribute vec3 a0; attribute vec4 a1; varying vec4 vc; void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); vc = a1; }",
		"precision mediump float; varying vec4 vc; void main() { gl_FragColor = vec4(vc.xyzw); }" );

	wglUXFRM = wgl.getUniformLocation(wglShader, "xfrm" );

	//Compile the shaders.
	wgl.useProgram(wglShader);

	//Get some vertex/color buffers, to put geometry in.
	wglABV = wgl.createBuffer();
	wglABC = wgl.createBuffer();

	//We're using two buffers, so just enable them, now.
	wgl.enableVertexAttribArray(0);
	wgl.enableVertexAttribArray(1);

	//Enable alpha blending
	wgl.enable(wgl.BLEND);
	wgl.blendFunc(wgl.SRC_ALPHA, wgl.ONE_MINUS_SRC_ALPHA);
}

//Do webgl work that must happen every frame.
function FrameStart()
{
	//Fixup canvas sizes
	if( fullscreen )
	{
		wgl.viewportWidth = canvas.width = window.innerWidth;
		wgl.viewportHeight = canvas.height = window.innerHeight;
	}
	
	//Make sure viewport and input to shader is correct.
	//We do this so we can pass literal coordinates into the shader.
	wgl.viewport( 0, 0, wgl.viewportWidth, wgl.viewportHeight );

	//Update geometry transform (Scale/shift)
	wgl.uniform4f( wglUXFRM, 
		1./wgl.viewportWidth, -1./wgl.viewportHeight,
		-0.5, 0.5);
}

function SystemStart( title, w, h )
{
	document.title = toUTF8( title );
	wgl.viewportWidth = canvas.width = w;
	wgl.viewportHeight = canvas.height = h;
}

//Buffered geometry system.
//This handles buffering a bunch of lines/segments, and using them all at once.
globalv = null;

function CNFGEmitBackendTrianglesJS( vertsF, colorsI, vertcount )
{
	const ab = wgl.ARRAY_BUFFER;
	wgl.bindBuffer(ab, wglABV);
	wgl.bufferData(ab, vertsF, wgl.DYNAMIC_DRAW);
	wgl.vertexAttribPointer(0, 3, wgl.FLOAT, false, 0, 0);
	wgl.bindBuffer(ab, wglABC);
	wgl.bufferData(ab, colorsI, wgl.DYNAMIC_DRAW);
	wgl.vertexAttribPointer(1, 4, wgl.UNSIGNED_BYTE, true, 0, 0);
	wgl.drawArrays(wgl.TRIANGLES, 0, vertcount );
	globalv = vertsF;
}

//This defines the list of imports, the things that C will be importing from Javascript.
//To use functions here, just call them.  Surprisingly, signatures justwork.
let imports = {
	env: {
		//Mapping our array buffer into the system.
		memory: memory,

		//Various draw-functions.
		CNFGEmitBackendTriangles : (vertsF, colorsI, vertcount )=>
		{
			//Take a float* and uint32_t* of vertices, and flat-render them.
			CNFGEmitBackendTrianglesJS(
				HEAPF32.slice(vertsF>>2,(vertsF>>2)+vertcount*3),
				HEAPU8.slice(colorsI,(colorsI)+vertcount*4),
				vertcount );
		},
		CNFGSetup : (title,w,h ) => {
			SystemStart( title, w, h );
			fullscreen = false;
		},
		CNFGSetupFullscreen : (title,w,h ) => {
			SystemStart( title, w, h );
			canvas.style = "position:absolute; top:0; left:0;"
			fullscreen = true;
		},
		CNFGClearFrameInternal: ( color ) => {
			wgl.clearColor( (color&0xff)/255., ((color>>8)&0xff)/255.,
				((color>>16)&0xff)/255., ((color>>24)&0xff)/255. ); 
			wgl.clear( wgl.COLOR_BUFFER_BIT | wgl.COLOR_DEPTH_BIT );
		},
		CNFGGetDimensions: (pw, ph) => {
			HEAP16[pw>>1] = canvas.width;
			HEAP16[ph>>1] = canvas.height;
		},
		OGGetAbsoluteTime : () => { return new Date().getTime()/1000.; },

		Add1 : (i) => { return i+1; }, //Super simple function for speed testing.

		//Tricky - math functions just automatically link through.
		sin : Math.sin, 
		cos : Math.cos,
		tan : Math.tan,
		sinf : Math.sin,
		cosf : Math.cos,
		tanf : Math.tan,

		//Quick-and-dirty debug.
		print: console.log,
		prints: (str) => { console.log(toUTF8(str)); },
	}
};

if( !RAWDRAW_USE_LOOP_FUNCTION )
{
	imports.bynsyncify = {
		//Any javascript functions which may unwind the stack should be placed here.
		CNFGSwapBuffersInternal: () => {
			if (!rendering) {
				// We are called in order to start a sleep/unwind.
				// Fill in the data structure. The first value has the stack location,
				// which for simplicity we can start right after the data structure itself.
				HEAPU32[DATA_ADDR >> 2] = DATA_ADDR + 8;
				// The end of the stack will not be reached here anyhow.
				HEAPU32[DATA_ADDR + 4 >> 2] = 1024|0;
				wasmExports.asyncify_start_unwind(DATA_ADDR);
				rendering = true;
				// Resume after the proper delay.
				requestAnimationFrame(function() {
					FrameStart();
					wasmExports.asyncify_start_rewind(DATA_ADDR);
					// The code is now ready to rewind; to start the process, enter the
					// first function that should be on the call stack.
					wasmExports.main();
				});
			} else {
				// We are called as part of a resume/rewind. Stop sleeping.
				wasmExports.asyncify_stop_rewind();
				rendering = false;
			}
		}
	}
}

if( RAWDRAW_NEED_BLITTER )
{
	let wglBlit = null;   //Blitting shader for texture
	let wglTex = null;    //Texture handle for blitting.
	let wglUXFRMBlit = null; //Uniform location for transform on blitter

	//We are not currently supporting the software renderer.
	//We load two shaders, the other is a texture shader, for blitting things.
	wglBlit = wgl_makeShader( 
		"uniform vec4 xfrm; attribute vec3 a0; attribute vec4 a1; varying vec2 tc; void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); tc = a1.xy; }",
		"precision mediump float; varying vec2 tc; uniform sampler2D tex; void main() { gl_FragColor = texture2D(tex,tc).wzyx;}" );

	wglUXFRMBlit = wgl.getUniformLocation(wglBlit, "xfrm" );

	imports.env.CNFGBlitImageInternal = (memptr, x, y, w, h ) => {
			if( w <= 0 || h <= 0 ) return;

			wgl.useProgram(wglBlit);

			//Most of the time we don't use textures, so don't initiate at start.
			if( wglTex == null )	wglTex = wgl.createTexture(); 

			wgl.activeTexture(wgl.TEXTURE0);
			const t2d = wgl.TEXTURE_2D;
			wgl.bindTexture(t2d, wglTex);

			//Note that unlike the normal color operation, we don't have an extra offset.
			wgl.uniform4f( wglUXFRMBlit,
				1./wgl.viewportWidth, -1./wgl.viewportHeight,
				-.5+x/wgl.viewportWidth, .5-y/wgl.viewportHeight );

			//These parameters are required.  Not sure why the defaults don't work.
			wgl.texParameteri(t2d, wgl.TEXTURE_WRAP_T, wgl.CLAMP_TO_EDGE);
			wgl.texParameteri(t2d, wgl.TEXTURE_WRAP_S, wgl.CLAMP_TO_EDGE);
			wgl.texParameteri(t2d, wgl.TEXTURE_MIN_FILTER, wgl.NEAREST);

			wgl.texImage2D(t2d, 0, wgl.RGBA, w, h, 0, wgl.RGBA,
				wgl.UNSIGNED_BYTE, new Uint8Array(memory.buffer,memptr,w*h*4) );

			CNFGEmitBackendTrianglesJS( 
				new Float32Array( [0,0,0, w,0,0,     w,h,0,       0,0,0,   w,h,0,       0,h,0 ] ),
				new Uint8Array( [0,0,0,0, 255,0,0,0, 255,255,0,0, 0,0,0,0, 255,255,0,0, 0,255,0,0] ),
				6 );

			wgl.useProgram(wglShader);
		};
}

{
	// Actually load the WASM blob.
	let blob = atob('${BLOB}');
	let array = new Uint8Array(new ArrayBuffer(blob.length));
	for(let i = 0; i < blob.length; i++) array[i] = blob.charCodeAt(i);

	WebAssembly.instantiate(array, imports).then(
		function(wa) { 
			instance = wa.instance;
			wasmExports = instance.exports;

			//Attach inputs.
			if( instance.exports.HandleMotion )
			{
				canvas.addEventListener('mousemove', e => { instance.exports.HandleMotion( e.offsetX, e.offsetY, e.buttons ); } );
				canvas.addEventListener('touchmove', e => { instance.exports.HandleMotion( e.touches[0].clientX, e.touches[0].clientY, 1 ); } );
			}

			if( instance.exports.HandleButton )
			{
				canvas.addEventListener('mouseup', e => { instance.exports.HandleButton( e.offsetX, e.offsetY, e.button, 0 ); return false; } );
				canvas.addEventListener('mousedown', e => { instance.exports.HandleButton( e.offsetX, e.offsetY, e.button, 1 ); return false; } );
			}

			if( instance.exports.HandleKey )
			{
				document.addEventListener('keydown', e => { instance.exports.HandleKey( e.keyCode, 1 ); } );
				document.addEventListener('keyup', e => { instance.exports.HandleKey( e.keyCode, 0 ); } );
			}


			//Actually invoke main().  Note that, upon "CNFGSwapBuffers" this will 'exit'
			//But, will get re-entered from the swapbuffers animation callback.
			instance.exports.main();
			
			if( RAWDRAW_USE_LOOP_FUNCTION )
			{
				function floop() {
					FrameStart();
					requestAnimationFrame(floop);
					// The code is now ready to rewind; to start the process, enter the
					// first function that should be on the call stack.
					wasmExports.loop();
				}
				floop();
			}
		 } );

	//Code here would continue executing, but this code is executed *before* main.
}

