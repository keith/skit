CC := clang
CFLAGS ?= -Weverything -I.

ifeq ($(shell uname),Darwin)
XCODE_PATH ?= $(shell xcode-select -p)
SOURCEKIT_DIRECTORY ?= $(XCODE_PATH)/Toolchains/XcodeDefault.xctoolchain/usr/lib/
CFLAGS += -F$(SOURCEKIT_DIRECTORY)
LDFLAGS ?= -Wl,-rpath,$(SOURCEKIT_DIRECTORY) -framework sourcekitd
else
SOURCEKIT_DIRECTORY ?= /usr/lib
LDFLAGS ?= -Wl,-rpath,$(SOURCEKIT_DIRECTORY),-rpath,/usr/lib/swift/linux,-PIC \
		   -L$(SOURCEKIT_DIRECTORY) -L/usr/lib/swift/linux \
		   -l sourcekitdInProc -l swiftCore
endif


EXECUTABLE ?= skit
OTHER_CFLAGS ?= -g
PREFIX ?= /usr/local/bin

HEADERS = $(wildcard *.h) $(wildcard */*.h)
SRCS = $(wildcard *.c) $(wildcard */*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(OTHER_CFLAGS)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(OTHER_CFLAGS) $(LDFLAGS)

.PHONY: release
release: OTHER_CFLAGS = -O3
release: clean $(EXECUTABLE)

.PHONY: install
install: release
	install $(EXECUTABLE) $(PREFIX)

.PHONY: uninstall
uninstall:
	rm "$(PREFIX)/$(EXECUTABLE)"

.PHONY: clean
clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJS)
	find . -name "*.pyc" -delete

.PHONY: test
test: $(EXECUTABLE)
	python -m unittest discover -s tests
