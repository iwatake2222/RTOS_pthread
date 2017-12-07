include ../makefile_config

TARGET = libThread.a

SRCS = pthread/libThreadThread.cpp pthread/libThreadMsg.cpp
SUBDIRS = pthread

# SRCS = pthreadClass/libThreadIf.cpp pthreadClass/libThreadMsg.cpp pthreadClass/libThreadThread.cpp
# SUBDIRS = pthreadClass

# SRCS = stdlib/libThreadIf.cpp stdlib/libThreadMsg.cpp stdlib/libThreadThread.cpp
# SUBDIRS = stdlib

INCLUDES = -I. -I..
SUBS = 

all: sub_target $(TARGET)

$(TARGET): $(OBJS) $(SUBS)
	$(AR) r $@ $(OBJS) $(SUBS)

.PHONY: sub_target
sub_target:
ifdef SUBDIRS
	# $(foreach subdir,$(SUBDIRS),cd $(subdir) && $(MAKE) && cd ../ &&) cd ./
endif

.PHONY: clean
clean:
	$(DEL) *.o *.a *.exe *.dll *.so
ifdef SUBDIRS
	# $(foreach subdir,$(SUBDIRS),cd $(subdir) && $(MAKE) clean && cd ../ &&) cd ./
	$(foreach subdir,$(SUBDIRS),cd $(subdir) && $(DEL) *.o *.a *.exe *.dll *.so && cd ../ &&) cd ./
endif

