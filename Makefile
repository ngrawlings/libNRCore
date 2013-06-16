NAME=libnrcore
CC=g++
DEFS = -DPACKAGE_NAME=\"myconfig\" -DPACKAGE_TARNAME=\"myconfig\" -DPACKAGE_VERSION=\"version-0.1\" -DPACKAGE_STRING=\"myconfig\ version-0.1\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_OPENSSL_RSA_H=1 -DHAVE_OPENSSL_PEM_H=1 -DHAVE_OPENSSL_ERR_H=1 -DHAVE_OPENSSL_X509_H=1 -DHAVE_OPENSSL_X509V3_H=1 -DHAVE_EVENT2_EVENT_STRUCT_H=1 -DHAVE_EVENT2_EVENT_H=1 -DHAVE_EVENT2_EVENT_COMPAT_H=1 -DHAVE_EVENT2_BUFFER_H=1 -DHAVE_LIBCRYPTO=1 -DHAVE_LIBEVENT=1 -DWITH_LIBCURL=1
CFLAGS=-m64 -c -Wall -g3 -gdwarf-2 -D DEBUG -I./ -I./src $(DEFS)
LDFLAGS=
SOURCES=$(shell for file in `find ./src -name *.cpp`;do echo $$file; done)
HEADERS=$(shell for file in `find ./src -name *.h`;do echo $$file; done)
OBJECTS=$(subst src,build,$(SOURCES:.cpp=.o))
BUILDPATH=./build
INSTALL_HEADER_PATH=/usr/local/include/$(NAME)
INSTALL_LIB_PATH=/usr/local/lib
DST_HEADERS=$(subst ./src,$(INSTALL_HEADER_PATH),$(HEADERS))
STATIC_LIBRARY=$(NAME).a

all: $(SOURCES) $(STATIC_LIBRARY)
	
$(STATIC_LIBRARY): $(OBJECTS)
	ar rcs $(BUILDPATH)/$@ $(addprefix $(BUILDPATH)/, $(notdir $(OBJECTS)))

$(BUILDPATH)/%.o:
	+@[ -d $(BUILDPATH) ] || mkdir -p $(BUILDPATH)
	$(CC) $(CFLAGS) ./$(subst build,src,$(@:.o=.cpp)) -o $(BUILDPATH)/$(notdir $@)

install:$(DST_HEADERS)
	mkdir -p $(INSTALL_HEADER_PATH)
	cp $(BUILDPATH)/$(STATIC_LIBRARY) $(INSTALL_LIB_PATH)/$(STATIC_LIBRARY)

$(DST_HEADERS):
	+@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	cp $(subst $(INSTALL_HEADER_PATH), ./src, $@) $@

clean:
	rm -Rf $(BUILDPATH)/*.o
	rm -f $(BUILDPATH)/$(STATIC_LIBRARY)

remove:
	rm -Rf $(INSTALL_HEADER_PATH)
	rm -f $(INSTALL_LIB_PATH)/$(STATIC_LIBRARY)

cleanconf:
	rm -Rf config.status
	rm -f config.log
	rm -f Makefile
