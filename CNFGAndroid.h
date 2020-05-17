#ifndef _CNFG_ANDROID_H
#define _CNFG_ANDROID_H

//This file contains the additional functions that are available on the Android platform.
//In order to build rawdraw for Android, please compile CNFGEGLDriver.c with -DANDROID

extern struct android_app * gapp;
void AndroidMakeFullscreen();
int AndroidHasPermissions(const char* perm_name);
void AndroidRequestAppPermissions(const char * perm);
void AndroidDisplayKeyboard(int pShow);
int AndroidGetUnicodeChar( int keyCode, int metaState );
void AndroidSendToBack( int param );

extern int android_sdk_version; //Derived at start from property ro.build.version.sdk
extern int android_width, android_height;
extern int UpdateScreenWithBitmapOffsetX;
extern int UpdateScreenWithBitmapOffsetY;

//You must implement these.
void HandleResume();
void HandleSuspend();


//Departures:

// HandleMotion's "mask" parameter is actually just an index, not a mask

// CNFGSetup / CNFGSetupFullScreen only controls whether or not the navigation
// decoration is removed.  Fullscreen means *full screen* To choose fullscreen
// or not fullscrene, modify, in your AndroidManifest.xml file, the application
// section to either contain or not contain:
//     android:theme="@android:style/Theme.NoTitleBar.Fullscreen"

#endif

