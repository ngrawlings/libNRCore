include $(CLEAR_VARS)

LOCAL_MODULE 		:= libnrcore
LOCAL_PATH			:= jni/libNRCore/$(LOCAL_MODULE)

ARCH_ARM			:= yes
APP_ABI 			:= armeabi

LIBNRCORE_FILES += $(LOCAL_PATH)/base/Object.cpp 

LIBNRCORE_FILES += $(LOCAL_PATH)/debug/Log.cpp 

LIBNRCORE_FILES += $(LOCAL_PATH)/event/EventBase.cpp

LIBNRCORE_FILES += $(LOCAL_PATH)/io/Stream.cpp

LIBNRCORE_FILES += $(LOCAL_PATH)/memory/ByteArray.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/memory/String.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/memory/StringList.cpp

LIBNRCORE_FILES += $(LOCAL_PATH)/threading/HighResTimer.cpp 
LIBNRCORE_FILES += $(LOCAL_PATH)/threading/Thread.cpp 
LIBNRCORE_FILES += $(LOCAL_PATH)/threading/Task.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/threading/Mutex.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/threading/TaskMutex.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/threading/ThreadWaitCondition.cpp

LIBNRCORE_FILES += $(LOCAL_PATH)/socket/Address.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/Buffer.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/Listener.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/Socket.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/Socks5.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/authentication/Socks5Auth.cpp
LIBNRCORE_FILES += $(LOCAL_PATH)/socket/authentication/Socks5AuthPlain.cpp

LIBNRCORE_FILES += $(LOCAL_PATH)/utils/ByteUtils.cpp

LOCAL_CPP_FEATURES := exceptions
LOCAL_CPPFLAGS := -fexceptions -I./libevent/include -I./libevent/android
LOCAL_CFLAGS		:= -g -O1 -DHAVE_CONFIG_H 

include $(BUILD_STATIC_LIBRARY)
