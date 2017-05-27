#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#include "CNFGFunctions.h"
#include "CNFGRasterizer.h"

int app_sw=-999, app_sh=-999;
BOOL inFullscreen = false;

id app_oglView;
id app_oglContext;

typedef struct
{
    GLfloat x,y;
} Vector2;

typedef struct
{
    GLfloat x,y,z,w;
} Vector4;

int CompileGLSLShader(const char *vert, const char *frag)
{
    GLint rt;
    GLsizei logLen;
    char log[4096];

    int my_vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(my_vertex_shader, 1, &vert, NULL);
    glCompileShader(my_vertex_shader);
    glGetShaderiv(my_vertex_shader, GL_COMPILE_STATUS, &rt);
    if (!rt) {
        glGetShaderInfoLog(my_vertex_shader, 4096, &logLen, log);
        printf("vert log (code %d): %s\n", rt, log);
        exit(1);
    }

    int my_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(my_fragment_shader, 1, &frag, NULL);
    glCompileShader(my_fragment_shader);
    glGetShaderiv(my_fragment_shader, GL_COMPILE_STATUS, &rt);
    if (!rt) {
        glGetShaderInfoLog(my_fragment_shader, 4096, &logLen, log);
        printf("frag log (code %d): %s\n", rt, log);
        exit(1);
    }
    
    int my_program = glCreateProgram();
    glAttachShader(my_program, my_vertex_shader);
    glAttachShader(my_program, my_fragment_shader);
    glBindFragDataLocation(my_program, 0, "fragColour");

    glLinkProgram(my_program);
    glGetShaderiv(my_program, GL_LINK_STATUS, &rt);
    if (!rt) {
        glGetShaderInfoLog(my_program, 4096, &logLen, log);
        printf("prog log: (code %d) %s\n", rt, log);
        exit(0);
    }

    return my_program;
}

GLuint shader_program;
GLuint vertexArrayObject;
GLuint vertexBuffer;

GLint positionUniform, positionAttribute, textureUniform;

GLuint frame_texture;

void loadBufferData()
{
    Vector4 vertexData[4] = {
        { .x=-1.0, .y=-1.0, .z=0.0, .w=1.0 },
        { .x=-1.0, .y= 1.0, .z=0.0, .w=1.0 },
        { .x= 1.0, .y= 1.0, .z=0.0, .w=1.0 },
        { .x= 1.0, .y=-1.0, .z=0.0, .w=1.0 }
    };
    
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector4), vertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray((GLuint)positionAttribute);
    glVertexAttribPointer((GLuint)positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), (const GLvoid *) 0);
}

GLuint make_texture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint) app_sw);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    return texture;
}

void oglInit()
{
    const char *vertex_shader="#version 150\n\
        in vec4 position;\n\
        out vec2 positionV;\n\
        void main (void)\n\
        {\n\
            positionV   = vec2(0.5) * (position.xy + vec2(1.0));\n\
            gl_Position = position;\n\
        }";
    
    const char *fragment_shader="#version 150\n\
        in vec2 positionV;\n\
        out vec4 fragColour;\n\
        uniform sampler2D picture;\n\
        void main(void)\n\
        {\n\
            fragColour = texture(picture, vec2(positionV.x, 1.0-positionV.y));\n\
        }";
    
    shader_program = CompileGLSLShader(vertex_shader, fragment_shader);

    textureUniform = glGetUniformLocation(shader_program, "picture");
    positionAttribute = glGetAttribLocation(shader_program, "position");
    loadBufferData();
    frame_texture = make_texture();
}

id app_menubar, app_appMenuItem, app_appMenu, app_appName, app_quitMenuItem, app_quitTitle, app_quitMenuItem, app_window;
NSRect frameRect;
NSAutoreleasePool *app_pool;
NSDate *app_currDate; 

NSOpenGLPixelFormat *pixelFormat;

void initApp(){
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    app_menubar = [[NSMenu new] autorelease];
    app_appMenuItem = [[NSMenuItem new] autorelease];
    [app_menubar addItem:app_appMenuItem];
    [NSApp setMainMenu:app_menubar];
    app_appMenu = [[NSMenu new] autorelease];
    app_appName = [[NSProcessInfo processInfo] processName];
    app_quitTitle = [@"Quit " stringByAppendingString:app_appName];
    app_quitMenuItem = [[[NSMenuItem alloc] initWithTitle:app_quitTitle
        action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [app_appMenu addItem:app_quitMenuItem];
    [app_appMenuItem setSubmenu:app_appMenu];

    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize    , 24                           ,
        NSOpenGLPFAAlphaSize    , 8                            ,
        NSOpenGLPFADoubleBuffer ,
        NSOpenGLPFAAccelerated  ,
        NSOpenGLPFANoRecovery   ,
        0
    };

    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
}

void CNFGSetupFullscreen( const char * WindowName, int screen_number )
{
    inFullscreen = YES;
    initApp();

    NSDictionary *fullScreenOptions = [NSDictionary dictionaryWithObjectsAndKeys: 
        [NSNumber numberWithInt: 
            (NSApplicationPresentationAutoHideMenuBar | NSApplicationPresentationAutoHideDock) ],
        NSFullScreenModeApplicationPresentationOptions, nil];

    NSScreen *appScreen = [[NSScreen screens] objectAtIndex:screen_number];
    frameRect = [appScreen frame];

    app_oglView = [[NSOpenGLView alloc] initWithFrame:frameRect  pixelFormat: pixelFormat];
    [app_oglView enterFullScreenMode:appScreen withOptions:fullScreenOptions];

    app_oglContext = [app_oglView openGLContext];
    [app_oglContext makeCurrentContext];

    CGSize app_size = frameRect.size;
    app_sw = app_size.width; app_sh = app_size.height;
    [NSApp finishLaunching];
    [NSApp updateWindows];

    oglInit();

    app_pool = [NSAutoreleasePool new];
}

void CNFGSetup( const char * WindowName, int sw, int sh )
{
    app_sw=sw; app_sh=sh;
    frameRect = NSMakeRect(0, 0, app_sw, app_sh);
    
    initApp();

    app_window = [[[NSWindow alloc] 
        initWithContentRect:frameRect
        styleMask:
                NSWindowStyleMaskBorderless | 
                NSWindowStyleMaskResizable | 
                NSWindowStyleMaskTitled | 
                NSWindowStyleMaskClosable | 
                NSWindowStyleMaskMiniaturizable 
        backing:NSBackingStoreBuffered defer:NO]
            autorelease];

    NSString *title = [[[NSString alloc] initWithCString: WindowName encoding: NSUTF8StringEncoding] autorelease];
    [app_window setTitle:title];

    app_oglView = [[NSOpenGLView alloc] initWithFrame: frameRect pixelFormat: pixelFormat];

    app_oglContext = [app_oglView openGLContext];
    [app_oglContext makeCurrentContext];

    [app_window setContentView:app_oglView];
    [app_window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [app_window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp finishLaunching];
    [NSApp updateWindows];

    oglInit();

    app_pool = [NSAutoreleasePool new];
}

#define XK_Left                          0xff51  /* Move left, left arrow */
#define XK_Up                            0xff52  /* Move up, up arrow */
#define XK_Right                         0xff53  /* Move right, right arrow */
#define XK_Down                          0xff54  /* Move down, down arrow */
#define KEY_UNDEFINED 255
#define KEY_LEFT_MOUSE 0

int app_mouseX=0, app_mouseY=0;

static int keycode(key)
{
    if (key < 256) return key;
    switch(key) {
        case 63232: return XK_Up;
        case 63233: return XK_Down;
        case 63234: return XK_Left;
        case 63235: return XK_Right;
    }
    return KEY_UNDEFINED;
}

void CNFGHandleInput()
{
    [app_pool release];
    app_pool = [NSAutoreleasePool new];
    // Quit if no open windows left
    if ([[NSApp windows] count] == 0) [NSApp terminate: nil];

    //----------------------
    // Peek at the next event
    //----------------------
    NSDate *app_currDate = [NSDate new];
    // If we have events, handle them!
    NSEvent *event;
    for (;(event = [NSApp
                    nextEventMatchingMask:NSEventMaskAny
                    untilDate:app_currDate
                    inMode:NSDefaultRunLoopMode
                    dequeue:YES]);)
    {
        NSPoint local_point;
        NSEventType type = [event type];
        switch (type)
        {
            case NSEventTypeKeyDown:
                for (int i=0; i<[event.characters length]; i++) {
                    unichar ch = [event.characters characterAtIndex: i];
                    HandleKey(keycode(ch), 1);
                }
                break;
                
            case NSEventTypeKeyUp:
                for (int i=0; i<[event.characters length]; i++) {
                    unichar ch = [event.characters characterAtIndex: i];
                    HandleKey(keycode(ch), 0);
                }
                break;
                    
            case NSEventTypeMouseMoved:
            case NSEventTypeLeftMouseDragged:
            case NSEventTypeRightMouseDragged:
            case NSEventTypeOtherMouseDragged:
                if (inFullscreen){
                    local_point = [NSEvent mouseLocation];
                } else {
                    if ([event window] == nil) break;
                    NSPoint event_location = event.locationInWindow;
                    local_point = [app_oglView convertPoint:event_location fromView:nil];
                }
                app_mouseX = fmax(fmin(local_point.x, app_sw), 0);
                // Y coordinate must be inversed?
                app_mouseY = fmax(fmin(app_sh - local_point.y, app_sh), 0);
                HandleMotion(app_mouseX, app_mouseY, [NSEvent pressedMouseButtons]);
                break;  

            case NSEventTypeLeftMouseDown:
            case NSEventTypeRightMouseDown:
            case NSEventTypeOtherMouseDown:
                // Button number start from 1?
                HandleButton(app_mouseX, app_mouseY, event.buttonNumber+1, 1); 
                break;
                    
            case NSEventTypeLeftMouseUp:
            case NSEventTypeRightMouseUp:
            case NSEventTypeOtherMouseUp:
                HandleButton(app_mouseX, app_mouseY, event.buttonNumber+1, 0);
                break;

            default:
                break;
        }
        [NSApp sendEvent:event];
    }
    [app_currDate release];
}

void CNFGGetDimensions( short * x, short * y )
{
    frameRect = [app_oglView frame];

    CGSize app_imageSize = frameRect.size;
    app_sw = app_imageSize.width;
    app_sh = app_imageSize.height;

    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint) app_sw);

    glViewport(0,0,app_sw,app_sh);
    *x = app_sw;
    *y = app_sh;
}

void CNFGUpdateScreenWithBitmap( unsigned long * data, int w, int h )
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader_program);

    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level of detail */
        GL_RGBA8,                    /* internal format */
        app_sw, app_sh, 0,           /* width, height, border */
        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,   /* external format, type */
        buffer                      /* pixels */
    );

    glUniform1i(textureUniform, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    [app_oglContext flushBuffer];
}
