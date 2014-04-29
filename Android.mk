include $(CLEAR_VARS)

LOCAL_MODULE 		:= libnrcore
LOCAL_PATH 			:= $(LOCAL_PATH)/libNRCore/libnrcore

ARCH_ARM			:= yes
APP_ABI 			:= armeabi

LOCAL_SRC_FILES += base/Object.cpp 

LOCAL_SRC_FILES += debug/Log.cpp 

LOCAL_SRC_FILES += event/EventBase.cpp

LOCAL_SRC_FILES += io/Stream.cpp

LOCAL_SRC_FILES += memory/ByteArray.cpp
LOCAL_SRC_FILES += memory/String.cpp
LOCAL_SRC_FILES += memory/StringList.cpp

LOCAL_SRC_FILES += threading/HighResTimer.cpp 
LOCAL_SRC_FILES += threading/Thread.cpp 
LOCAL_SRC_FILES += threading/Task.cpp
LOCAL_SRC_FILES += threading/Mutex.cpp
LOCAL_SRC_FILES += threading/TaskMutex.cpp
LOCAL_SRC_FILES += threading/ThreadWaitCondition.cpp

LOCAL_SRC_FILES += socket/Address.cpp
LOCAL_SRC_FILES += socket/Buffer.cpp
LOCAL_SRC_FILES += socket/Listener.cpp
LOCAL_SRC_FILES += socket/Socket.cpp
LOCAL_SRC_FILES += socket/Socks5.cpp
LOCAL_SRC_FILES += socket/UdpSocket.cpp
LOCAL_SRC_FILES += socket/authentication/Socks5Auth.cpp
LOCAL_SRC_FILES += socket/authentication/Socks5AuthPlain.cpp

LOCAL_SRC_FILES += ../platform/linux/socket/Interfaces.cpp

LOCAL_SRC_FILES += utils/ByteUtils.cpp

LOCAL_CPP_FEATURES := exceptions
LOCAL_CPPFLAGS := -fexceptions $(INCLUDES_DIRS) 
LOCAL_CFLAGS		:= -g -O1 -DHAVE_CONFIG_H 

include $(BUILD_STATIC_LIBRARY)
