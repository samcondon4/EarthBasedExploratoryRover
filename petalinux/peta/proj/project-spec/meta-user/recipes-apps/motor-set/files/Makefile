APP = motor-set

# Add any other object files to this list below
APP_OBJS = motor-set.o
APP_OBJS += uart.o

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

