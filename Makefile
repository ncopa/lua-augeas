
LUA_LIBDIR = /usr/lib/lua/5.1

VERSION = 0.1.1
GIT_REV		:= $(shell test -d .git && git describe || echo exported)
ifneq ($(GIT_REV), exported)
FULL_VERSION    := $(GIT_REV)
FULL_VERSION    := $(patsubst v%,%,$(FULL_VERSION))
else
FULL_VERSION    := $(VERSION)
endif

AUGEAS_LIBS= $(shell pkg-config --libs augeas)
OBJS = laugeas.o
LIBS = $(AUGEAS_LIBS)

CFLAGS ?= -g -Wall -Werror
CFLAGS += -fPIC
CFLAGS += -DVERSION=\"$(FULL_VERSION)\"

LDFLAGS += -L/lib

all:	augeas.so


augeas.so: $(OBJS)
	$(CC) $(LDFLAGS) -o $@  -fPIC -shared $^ $(LIBS)

clean:
	rm -f augeas.so $(OBJS)


