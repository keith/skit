CC := clang
CFLAGS ?= -Weverything -I.

ifeq ($(shell uname),Darwin)
LDFLAGS ?=
else
LDFLAGS ?= -Wl,-PIC,-ldl,-rpath,/usr/lib/swift/linux,-lswiftCore,-L/usr/lib/swift/linux
endif

EXECUTABLE ?= skit
OTHER_CFLAGS ?= -g
PREFIX ?= /usr/local/bin

HEADERS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

%.o: %.c $(HEADERS) sourcekitd/sourcekitd.h
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

.PHONY: format
format: $(SRCS) $(HEADERS)
	clang-format -i $(SRCS) $(HEADERS)

.PHONY: check-format
check-format: $(SRCS) $(HEADERS)
	@which clang-format
	set -o pipefail; \
		! clang-format -output-replacements-xml $(SRCS) $(HEADERS) \
		| grep "<replacement "
