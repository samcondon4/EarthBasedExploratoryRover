APP = eber-run

# Add any other object files to this list below
APP_OBJS = eber-run.o
APP_OBJS += MotorComChannel.o

LDLIBS += -lpthread

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)
