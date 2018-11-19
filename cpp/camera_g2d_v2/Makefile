APPNAME			= camera-video-g2d-845
DESTDIR			= .


TARGET_PATH_LIB = $(ROOTFS)/usr/lib
TARGET_PATH_INCLUDE = $(ROOTFS)/usr/include
#COMMON_DIR=../common

BIN_TUT = $(DESTDIR)/$(APPNAME)

CFLAGS_TUT = $(CFLAGS) $(CFLAGS_EGL)

CFLAGS_TUT += \
    -DLINUX \
	-DEGL_API_FB \

CFLAGS_TUT += \
	-I$(TARGET_PATH_INCLUDE) \
#	-I$(COMMON_DIR)/inc 
#	-I$(TARGET_PATH_INCLUDE)/gstreamer-1.0 \
#	-I$(TARGET_PATH_INCLUDE)/glib-2.0 \
#	-I$(ROOTFS)/usr/lib/glib-2.0/include \
#	-I${BUILDROOT}/usr/include/libxml2
	
	

OBJECTS_TUT += \
       my.o 



DEPS_TUT = -lstdc++ -lm  -lg2d   -lpthread -ldl -Wl,--library-path=$(TARGET_PATH_LIB),-rpath-link=$(TARGET_PATH_LIB)

$(BIN_TUT) : $(OBJECTS_TUT)
	@echo " LD " $@
	$(QUIET)$(CC) -o $(BIN_TUT) $(OBJECTS_TUT) $(DEPS_TUT)

%.o : %.c
	@echo " CC " $@
	$(QUIET)$(CC) $(CFLAGS_TUT) -MMD -c $< -o $@

%.o : %.cpp
	@echo " CXX " $@
	$(QUIET)$(CC) $(CFLAGS_TUT) -MMD -c $< -o $@

clean:
	rm -f $(OBJECTS_TUT) $(OBJECTS_TUT:.o=.d) $(BIN_TUT)
	
install:
	cp -f $(APPNAME)  $(DESTDIR)/.
	
	
-include $(OBJECTS_TUT:.o=.d)
