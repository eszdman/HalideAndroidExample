#include <jni.h>
#include <string>
#include "sample.h"
#include "HalideRuntime.h"
#include "HalideBuffer.h"
#include "HalideRuntimeOpenCL.h"
#include <android/log.h>
#include <dlfcn.h>
#include <xdl.h>

#define  LOG_TAG    "Halide Sample Native"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

void* loadCL(){
    const char* openclLibPath[] = {
            // Typical libOpenCL location
            "/odm/lib64/libOpenCL.so",
            "/system/lib64/libOpenCL.so",
            "/system/lib64/egl/libOpenCL.so",
            "/system/vendor/lib64/libOpenCL.so",
            "/system/vendor/lib64/egl/libOpenCL.so",
            "libOpenCL.so",
            "/system/vendor/lib/libOpenCL.so",
            "/system/lib/libOpenCL.so",
            "/system/lib/egl/libOpenCL.so",
            "/system/vendor/lib/egl/libOpenCL.so",
            // Qualcomm Adreno A3xx
            "/system/lib/libllvm-a3xx.so",
            // ARM Mali series
            "/system/lib64/libGLES_mali.so",
            "/system/lib64/egl/libGLES_mali.so",
            "/system/vendor/lib64/libGLES_mali.so",
            "/system/vendor/lib64/egl/libGLES_mali.so",
            "/system/lib/libGLES_mali.so",
            "/system/lib/egl/libGLES_mali.so",
            "/system/vendor/lib/libGLES_mali.so",
            "/system/vendor/lib/egl/libGLES_mali.so",
            // Imagination PowerVR Series
            "/system/lib64/libPVROCL.so",
            "/system/lib64/egl/libPVROCL.so",
            "/system/vendor/lib64/libPVROCL.so",
            "/system/vendor/lib64/egl/libPVROCL.so",
            "/system/lib/libPVROCL.so",
            "/system/lib/egl/libPVROCL.so",
            "/system/vendor/lib/libPVROCL.so",
            "/system/vendor/lib/egl/libPVROCL.so",
            // Last try
            "libGLES_mali.so",
            "libPVROCL.so"};
    void* handle;
    for (auto & i : openclLibPath) {
        LOGD("Try:%s\n",i);
        //Should be xdl
        if ((handle = xdl_open(i,XDL_TRY_FORCE_LOAD))) {
            const char* loaded = i;
            LOGD("Loaded OpenCL library:%s %p\n", loaded, handle);
            return handle;
        }
    }
    return handle;
}
bool xdl = false;
void* esOpen(const char *name){
    if(std::string(name) == "libOpenCL.so" || std::string(name) == "/System/Library/Frameworks/OpenCL.framework/OpenCL"){
        return loadCL();
    } else {
        return dlopen(name,RTLD_NOW);
    }
}

void* esSym(void* handle, const char *name){
    //LOGD("Opencl sym: %s",name);
    return xdl_sym(handle,name, nullptr);
}
void* sym(const char *name){
    LOGD("Sym: %s",name);
    return nullptr;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_halideandroidexample_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    halide_set_custom_load_library(esOpen);
    halide_set_custom_get_library_symbol(esSym);
    halide_set_custom_get_symbol(sym);
    halide_load_library("libOpenCL.so");
    halide_set_error_handler([](void *user_context, const char *msg) {
        LOGE("Halide error: %s\n", msg);
    });
    halide_set_custom_print([](void *user_context, const char *msg) {
        LOGD("Halide debug: %s\n", msg);
    });
    std::string hello = "Hello from C++ Halide! ";
    Halide::Runtime::Buffer<uint16_t> buff1(256);
    Halide::Runtime::Buffer<uint16_t> buff2(256);
    for(int i =0; i<256; i++){
        buff1(i) = i;
    }
    LOGD("Halide buffer write");
    LOGD("Halide device type %s", halide_opencl_get_device_type(nullptr));
    halide_opencl_set_device_type("gpu");
    bool zerocopy = false;
    bool profile = true;
    if(zerocopy)
        buff1.set_host_dirty(false);
    else {
        LOGD("Halide copy to device");
        //buff1.copy_to_device(halide_opencl_device_interface());
        buff1.set_host_dirty(false);
    }
    if(profile){
        LOGD("Halide execute");
        for(int i =0; i<99;i++)
            sample(buff1,buff2);
    }
    sample(buff1,buff2);
    if(zerocopy)
        buff2.set_device_dirty(false);
    else {
        LOGD("Halide copy to host");
        buff2.copy_to_host();
    }
    LOGD("Create string");
    for(int i =0; i<256; i++){
        hello += std::to_string(buff2(i)) + ", ";
    }
    halide_profiler_report(nullptr);
    return env->NewStringUTF(hello.c_str());
}