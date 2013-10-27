/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_openni_android_OpenNIView */

#ifndef _Included_org_openni_android_OpenNIView
#define _Included_org_openni_android_OpenNIView
#ifdef __cplusplus
extern "C" {
#endif
#undef org_openni_android_OpenNIView_NO_ID
#define org_openni_android_OpenNIView_NO_ID -1L
#undef org_openni_android_OpenNIView_VISIBLE
#define org_openni_android_OpenNIView_VISIBLE 0L
#undef org_openni_android_OpenNIView_INVISIBLE
#define org_openni_android_OpenNIView_INVISIBLE 4L
#undef org_openni_android_OpenNIView_GONE
#define org_openni_android_OpenNIView_GONE 8L
#undef org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_LOW
#define org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_LOW 524288L
#undef org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_HIGH
#define org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_HIGH 1048576L
#undef org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_AUTO
#define org_openni_android_OpenNIView_DRAWING_CACHE_QUALITY_AUTO 0L
#undef org_openni_android_OpenNIView_SCROLLBARS_INSIDE_OVERLAY
#define org_openni_android_OpenNIView_SCROLLBARS_INSIDE_OVERLAY 0L
#undef org_openni_android_OpenNIView_SCROLLBARS_INSIDE_INSET
#define org_openni_android_OpenNIView_SCROLLBARS_INSIDE_INSET 16777216L
#undef org_openni_android_OpenNIView_SCROLLBARS_OUTSIDE_OVERLAY
#define org_openni_android_OpenNIView_SCROLLBARS_OUTSIDE_OVERLAY 33554432L
#undef org_openni_android_OpenNIView_SCROLLBARS_OUTSIDE_INSET
#define org_openni_android_OpenNIView_SCROLLBARS_OUTSIDE_INSET 50331648L
#undef org_openni_android_OpenNIView_KEEP_SCREEN_ON
#define org_openni_android_OpenNIView_KEEP_SCREEN_ON 67108864L
#undef org_openni_android_OpenNIView_SOUND_EFFECTS_ENABLED
#define org_openni_android_OpenNIView_SOUND_EFFECTS_ENABLED 134217728L
#undef org_openni_android_OpenNIView_HAPTIC_FEEDBACK_ENABLED
#define org_openni_android_OpenNIView_HAPTIC_FEEDBACK_ENABLED 268435456L
#undef org_openni_android_OpenNIView_FOCUSABLES_ALL
#define org_openni_android_OpenNIView_FOCUSABLES_ALL 0L
#undef org_openni_android_OpenNIView_FOCUSABLES_TOUCH_MODE
#define org_openni_android_OpenNIView_FOCUSABLES_TOUCH_MODE 1L
#undef org_openni_android_OpenNIView_FOCUS_BACKWARD
#define org_openni_android_OpenNIView_FOCUS_BACKWARD 1L
#undef org_openni_android_OpenNIView_FOCUS_FORWARD
#define org_openni_android_OpenNIView_FOCUS_FORWARD 2L
#undef org_openni_android_OpenNIView_FOCUS_LEFT
#define org_openni_android_OpenNIView_FOCUS_LEFT 17L
#undef org_openni_android_OpenNIView_FOCUS_UP
#define org_openni_android_OpenNIView_FOCUS_UP 33L
#undef org_openni_android_OpenNIView_FOCUS_RIGHT
#define org_openni_android_OpenNIView_FOCUS_RIGHT 66L
#undef org_openni_android_OpenNIView_FOCUS_DOWN
#define org_openni_android_OpenNIView_FOCUS_DOWN 130L
#undef org_openni_android_OpenNIView_MEASURED_SIZE_MASK
#define org_openni_android_OpenNIView_MEASURED_SIZE_MASK 16777215L
#undef org_openni_android_OpenNIView_MEASURED_STATE_MASK
#define org_openni_android_OpenNIView_MEASURED_STATE_MASK -16777216L
#undef org_openni_android_OpenNIView_MEASURED_HEIGHT_STATE_SHIFT
#define org_openni_android_OpenNIView_MEASURED_HEIGHT_STATE_SHIFT 16L
#undef org_openni_android_OpenNIView_MEASURED_STATE_TOO_SMALL
#define org_openni_android_OpenNIView_MEASURED_STATE_TOO_SMALL 16777216L
#undef org_openni_android_OpenNIView_OVER_SCROLL_ALWAYS
#define org_openni_android_OpenNIView_OVER_SCROLL_ALWAYS 0L
#undef org_openni_android_OpenNIView_OVER_SCROLL_IF_CONTENT_SCROLLS
#define org_openni_android_OpenNIView_OVER_SCROLL_IF_CONTENT_SCROLLS 1L
#undef org_openni_android_OpenNIView_OVER_SCROLL_NEVER
#define org_openni_android_OpenNIView_OVER_SCROLL_NEVER 2L
#undef org_openni_android_OpenNIView_STATUS_BAR_VISIBLE
#define org_openni_android_OpenNIView_STATUS_BAR_VISIBLE 0L
#undef org_openni_android_OpenNIView_STATUS_BAR_HIDDEN
#define org_openni_android_OpenNIView_STATUS_BAR_HIDDEN 1L
#undef org_openni_android_OpenNIView_SCROLLBAR_POSITION_DEFAULT
#define org_openni_android_OpenNIView_SCROLLBAR_POSITION_DEFAULT 0L
#undef org_openni_android_OpenNIView_SCROLLBAR_POSITION_LEFT
#define org_openni_android_OpenNIView_SCROLLBAR_POSITION_LEFT 1L
#undef org_openni_android_OpenNIView_SCROLLBAR_POSITION_RIGHT
#define org_openni_android_OpenNIView_SCROLLBAR_POSITION_RIGHT 2L
#undef org_openni_android_OpenNIView_LAYER_TYPE_NONE
#define org_openni_android_OpenNIView_LAYER_TYPE_NONE 0L
#undef org_openni_android_OpenNIView_LAYER_TYPE_SOFTWARE
#define org_openni_android_OpenNIView_LAYER_TYPE_SOFTWARE 1L
#undef org_openni_android_OpenNIView_LAYER_TYPE_HARDWARE
#define org_openni_android_OpenNIView_LAYER_TYPE_HARDWARE 2L
#undef org_openni_android_OpenNIView_RENDERMODE_WHEN_DIRTY
#define org_openni_android_OpenNIView_RENDERMODE_WHEN_DIRTY 0L
#undef org_openni_android_OpenNIView_RENDERMODE_CONTINUOUSLY
#define org_openni_android_OpenNIView_RENDERMODE_CONTINUOUSLY 1L
#undef org_openni_android_OpenNIView_DEBUG_CHECK_GL_ERROR
#define org_openni_android_OpenNIView_DEBUG_CHECK_GL_ERROR 1L
#undef org_openni_android_OpenNIView_DEBUG_LOG_GL_CALLS
#define org_openni_android_OpenNIView_DEBUG_LOG_GL_CALLS 2L
/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeCreate
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_openni_android_OpenNIView_nativeCreate
  (JNIEnv *, jclass);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeDestroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeDestroy
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeSetAlphaValue
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeSetAlphaValue
  (JNIEnv *, jclass, jlong, jint);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeGetAlphaValue
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_openni_android_OpenNIView_nativeGetAlphaValue
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeOnSurfaceCreated
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeOnSurfaceCreated
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeUpdate
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeUpdate
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeClear
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeClear
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_openni_android_OpenNIView
 * Method:    nativeOnDraw
 * Signature: (JIIII)V
 */
JNIEXPORT void JNICALL Java_org_openni_android_OpenNIView_nativeOnDraw
  (JNIEnv *, jclass, jlong, jint, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
