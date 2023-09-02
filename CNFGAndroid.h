#ifndef _CNFG_ANDROID_H
#define _CNFG_ANDROID_H

//This file contains the additional functions that are available on the Android platform.
//In order to build rawdraw for Android, please compile CNFGEGLDriver.c with -DANDROID

// Tricky: Android headers are confused by c++ if linking statically.
#ifdef __cplusplus
extern "C" {
	int __system_property_get(const char* __name, char* __value);
};
#endif

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
extern void (*HandleCustomEventCallback)();
extern void (*HandleWindowTermination)();


// If you need them, these are the names of raw EGL symbols.
//extern EGLDisplay egl_display;
//extern EGLSurface egl_surface;
//extern EGLContext egl_context;
//extern EGLConfig  egl_config;


//You must implement these.
void HandleResume();
void HandleSuspend();


//Departures:

// HandleMotion's "mask" parameter is actually just an index, not a mask

// CNFGSetup / CNFGSetupFullScreen only controls whether or not the navigation
// decoration is removed.  Fullscreen means *full screen* To choose fullscreen
// or not fullscrene, modify, in your AndroidManifest.xml file, the application
// section to either contain or not contain:
//	 android:theme="@android:style/Theme.NoTitleBar.Fullscreen"


// For debugging:

#if defined( ANDROID ) && !defined( __cplusplus )

#include <jni.h>

static inline void PrintClassOfObject( jobject bundle )
{
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;
	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);

	jclass myclass = env->GetObjectClass( envptr, bundle );
	jmethodID mid = env->GetMethodID( envptr, myclass, "getClass", "()Ljava/lang/Class;");
	jobject clsObj = env->CallObjectMethod( envptr, bundle, mid );
	jclass clazzz = env->GetObjectClass( envptr, clsObj );
	mid = env->GetMethodID(envptr, clazzz, "getName", "()Ljava/lang/String;");
	jstring strObj = (jstring)env->CallObjectMethod( envptr, clsObj, mid);
	const char * name = env->GetStringUTFChars( envptr, strObj, 0);
	printf( "Class type: %s\n", name );

	env->DeleteLocalRef( envptr, myclass );
	env->DeleteLocalRef( envptr, clsObj );
	env->DeleteLocalRef( envptr, clazzz );
	env->ReleaseStringUTFChars(envptr, strObj, name);
	env->DeleteLocalRef( envptr, strObj );
}

static inline void PrintObjectString( jobject bundle )
{
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;

	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);

	jclass myclass = env->GetObjectClass( envptr, bundle );
	jmethodID toStringMethod = env->GetMethodID( envptr, myclass, "toString", "()Ljava/lang/String;");
	jstring strObjDescr = (jstring)env->CallObjectMethod( envptr, bundle, toStringMethod);
	const char *descr = env->GetStringUTFChars( envptr, strObjDescr, 0);
	printf( "String: %s\n", descr );

	env->DeleteLocalRef( envptr, myclass );
	env->ReleaseStringUTFChars( envptr, strObjDescr, descr );
	env->DeleteLocalRef( envptr, strObjDescr );
}


static inline void DumpObjectClassProperties( jobject objToDump )
{
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;

	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);

	jclass mpclass = env->GetObjectClass( envptr, objToDump );

	jmethodID midGetClass = env->GetMethodID( envptr, mpclass, "getClass", "()Ljava/lang/Class;");
	jclass ClassClass = env->FindClass(envptr, "java/lang/Class");
	jobject clsObj = env->CallObjectMethod( envptr, objToDump, midGetClass);
	
	jmethodID gnid = env->GetMethodID(envptr, ClassClass, "getName", "()Ljava/lang/String;");
	jstring nameObj = (jstring)env->CallObjectMethod( envptr, clsObj, gnid);
	const char *name = env->GetStringUTFChars( envptr, nameObj, 0);
	printf( "Class Name: %s\n", name );
	env->ReleaseStringUTFChars(envptr, nameObj, name);
	env->DeleteLocalRef( envptr, nameObj );
	
	jmethodID getMethodsMethod = env->GetMethodID( envptr, ClassClass, "getMethods","()[Ljava/lang/reflect/Method;");
	jobject jobjArray = env->CallObjectMethod( envptr, clsObj, getMethodsMethod );

	jclass MethodType = env->FindClass(envptr, "java/lang/reflect/Method");
	jclass TypeType = env->FindClass(envptr, "java/lang/reflect/Type");
	jmethodID tnamemid = env->GetMethodID( envptr, TypeType, "getTypeName", "()Ljava/lang/String;" );

	jmethodID getFieldsMethod = env->GetMethodID( envptr, ClassClass, "getFields","()[Ljava/lang/reflect/Field;");
	jobject jobjArrayFields = env->CallObjectMethod( envptr, clsObj, getFieldsMethod );

	jsize len = env->GetArrayLength(envptr, jobjArray);
	jsize i;
	printf( "Methods:\n" );

	for (i = 0 ; i < len ; i++) {
		jobject _strMethod = env->GetObjectArrayElement( envptr, jobjArray, i ) ;
		jclass _methodClazz = env->GetObjectClass(envptr, _strMethod) ;
		jmethodID mid = env->GetMethodID(envptr, _methodClazz , "getName", "()Ljava/lang/String;" );
		jmethodID getGenericParameterTypes = env->GetMethodID( envptr, _methodClazz, "getGenericParameterTypes","()[Ljava/lang/reflect/Type;");
		jmethodID getReturnTypeMethod = env->GetMethodID( envptr, _methodClazz, "getGenericReturnType","()Ljava/lang/reflect/Type;");
		jstring _name = (jstring)env->CallObjectMethod( envptr, _strMethod , mid ) ;
		const char *str = env->GetStringUTFChars(envptr, _name, 0);
		printf("  %s( ", str);
		
		jobject types = env->CallObjectMethod( envptr, _strMethod, getGenericParameterTypes );
		jsize mlen = env->GetArrayLength(envptr, types);
		jsize mi;
		for( mi = 0; mi < mlen; mi++ )
		{
			jobject typeo = env->GetObjectArrayElement( envptr, types, mi );
			jstring _tn = (jstring)env->CallObjectMethod( envptr, typeo, tnamemid );
			const char * str = env->GetStringUTFChars(envptr, _tn, 0);
			printf("%s%s ", str, (mi == mlen-1)?"":"," );
			env->ReleaseStringUTFChars(envptr, _tn, str);
			env->DeleteLocalRef( envptr, _tn );
			env->DeleteLocalRef( envptr, typeo );
		}

		jobject rtype = env->CallObjectMethod( envptr, _strMethod, getReturnTypeMethod );
		jstring _tn = (jstring)env->CallObjectMethod( envptr, rtype, tnamemid );
		const char * strret = env->GetStringUTFChars(envptr, _tn, 0);
		printf(") -> %s\n", strret);
		env->ReleaseStringUTFChars(envptr, _tn, strret);
		env->DeleteLocalRef( envptr, _tn );
		env->DeleteLocalRef( envptr, rtype );
		
		
		env->DeleteLocalRef( envptr, types );
		env->ReleaseStringUTFChars(envptr, _name, str);
		env->DeleteLocalRef( envptr, _methodClazz );
		env->DeleteLocalRef( envptr, _strMethod );
		env->DeleteLocalRef( envptr, _name );
	}
	len = env->GetArrayLength(envptr, jobjArrayFields);
	printf( "Fields:\n" );
	for ( i = 0; i < len; i++) {
		jobject _strField = env->GetObjectArrayElement( envptr, jobjArrayFields, i) ;
		jclass _methodClazz = env->GetObjectClass(envptr, _strField );
		jmethodID mid = env->GetMethodID(envptr, _methodClazz , "getName", "()Ljava/lang/String;") ;
		jstring _name = (jstring)env->CallObjectMethod( envptr, _strField , mid ) ;
		const char *str = env->GetStringUTFChars(envptr, _name, 0);
		
		jmethodID getTypeMethod = env->GetMethodID( envptr, _methodClazz, "getGenericType","()Ljava/lang/reflect/Type;");
		jobject rtype = env->CallObjectMethod( envptr, _strField, getTypeMethod );
		jstring _tn = (jstring)env->CallObjectMethod( envptr, rtype, tnamemid );
		const char * strret = env->GetStringUTFChars(envptr, _tn, 0);

		printf("  %s -> %s\n", str, strret );
		env->ReleaseStringUTFChars(envptr, _name, str);
		env->ReleaseStringUTFChars(envptr, _tn, strret);
		env->DeleteLocalRef( envptr, _methodClazz );
		env->DeleteLocalRef( envptr, _strField );
		env->DeleteLocalRef( envptr, _name );
		env->DeleteLocalRef( envptr, _tn );
	}
	env->DeleteLocalRef( envptr, TypeType );
	env->DeleteLocalRef( envptr, MethodType );
	env->DeleteLocalRef( envptr, clsObj );
	env->DeleteLocalRef( envptr, ClassClass );
	env->DeleteLocalRef( envptr, jobjArrayFields );
	env->DeleteLocalRef( envptr, jobjArray );

	env->DeleteLocalRef( envptr, mpclass );
}
#endif

#endif

