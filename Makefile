
PKGCONFIG ?= pkg-config

VERSION = 0.1.1
GIT_REV		:= $(shell test -d .git && git describe || echo exported)
ifneq ($(GIT_REV), exported)
FULL_VERSION    := $(GIT_REV)
FULL_VERSION    := $(patsubst v%,%,$(FULL_VERSION))
else
FULL_VERSION    := $(VERSION)
endif

LUAPC := $(shell for pc in lua lua5.1; do \
			$(PKGCONFIG) --exists $$pc && echo $$pc && break; \
		done)

LUA_VERSION := $(shell $(PKGCONFIG) --variable=V $(LUAPC))
INSTALL_CMOD := $(shell $(PKGCONFIG) --variable=INSTALL_CMOD $(LUAPC))


AUGEAS_LIBS := $(shell $(PKGCONFIG) --libs augeas)
AUGEAS_CFLAGS := $(shell $(PKGCONFIG) --cflags augeas)
OBJS = laugeas.o
LIBS = $(AUGEAS_LIBS)

CFLAGS ?= -g -Wall -Werror
CFLAGS += -fPIC
CFLAGS += -DVERSION=\"$(FULL_VERSION)\"

LDFLAGS += -L/lib

all:	augeas.so

laugeas.o: CFLAGS+=$(AUGEAS_CFLAGS)

augeas.so: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@  -fPIC -shared $^ $(LIBS)

clean:
	rm -f augeas.so $(OBJS)

install: augeas.so
	install -D augeas.so $(DESTDIR)$(INSTALL_CMOD)/augeas.so

