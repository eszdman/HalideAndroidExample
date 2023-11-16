#include <jni.h>
#include <string>
#include "sample.h"
#include "HalideRuntime.h"
#include "HalideBuffer.h"
#include <android/log.h>

#define  LOG_TAG    "Halide Sample Native"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_halideandroidexample_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    halide_set_error_handler([](void *user_context, const char *msg) {
        LOGE("Halide error: %s\n", msg);
    });
    std::string hello = "Hello from C++ Halide! ";
    Halide::Runtime::Buffer<uint16_t> buff1(256);
    Halide::Runtime::Buffer<uint16_t> buff2(256);
    for(int i =0; i<256; i++){
        buff1(i) = i;
    }
    bool zerocopy = false;
    if(zerocopy)
        buff1.set_host_dirty(false);
    sample(buff1,buff2);
    if(zerocopy)
        buff2.set_device_dirty(false);
    else {
        buff2.copy_to_host();
    }
    for(int i =0; i<256; i++){
        hello += std::to_string(buff2(i)) + ", ";
    }
    return env->NewStringUTF(hello.c_str());
}