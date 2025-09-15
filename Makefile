CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -D_GNU_SOURCE
LDFLAGS = 

SRCDIR = src
APPLETDIR = $(SRCDIR)/applets

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/utils.c \
       $(APPLETDIR)/cat.c \
       $(APPLETDIR)/ls.c \
       $(APPLETDIR)/cp.c \
       $(APPLETDIR)/echo.c \
       $(APPLETDIR)/pwd.c \
       $(APPLETDIR)/mkdir.c \
       $(APPLETDIR)/rm.c \
       $(APPLETDIR)/mv.c \
       $(APPLETDIR)/touch.c \
       $(APPLETDIR)/head.c \
       $(APPLETDIR)/tail.c

OBJS = $(SRCS:.c=.o)
TARGET = xylenutils

APPLETS = cat cp echo head ls mkdir mv pwd rm tail touch

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

install: $(TARGET)
	install -d $(DESTDIR)/usr/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/bin/
	cd $(DESTDIR)/usr/bin && \
	for applet in $(APPLETS); do \
		ln -sf $(TARGET) $$applet; \
	done

clean:
	rm -f $(OBJS) $(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)
