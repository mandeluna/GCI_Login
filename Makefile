# Usage:
# Windows (MinGW/Cygwin): make
# Linux: make

APP_NAME = gs_login
SRC = gci_test.c
OBJ = $(SRC:.c=.o)

# GemStone Versioning
GS_VER = 3.7.4.3-64

CXX = g++

# Detect OS
ifeq ($(OS),Windows_NT)
    TARGET = $(APP_NAME).exe
    RM = del /Q
else
    TARGET = $(APP_NAME)
    RM = rm -f
endif

# The following flags are recommended for compilation:
WARNFLAGS = -Wformat -Wtrigraphs -Wcomment -Wno-aggregate-return -Wswitch \
	-Wshadow -Wunused-value -Wunused-variable -Wunused-label	  \
	-Wno-unused-function -Wchar-subscripts -Wmissing-braces		  \
	-Wmissing-declarations -Wmultichar -Wparentheses -Wsign-compare   \
	-Wsign-promo -Wwrite-strings -Wreturn-type 			  \
	-Wno-format-truncation -Wuninitialized

CXXFLAGS = -fmessage-length=0 -fcheck-new -ggdb -m64 -pipe		  \
	-D_REENTRANT -D_GNU_SOURCE -pthread -fPIC -fno-strict-aliasing	  \
	-fno-exceptions -I$(GEMSTONE)/include -x c++ -g

LINKFLAGS = $(GEMSTONE)/lib/gcirtlobj.o -m64				  \
	-Wl,-Bdynamic,--no-as-needed -lpthread -Wl,--as-needed -lcrypt    \
	-ldl -lc -lm -lrt -z noexecstack -Wl,-traditional -Wl,-z,lazy

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LINKFLAGS) -o $(TARGET)

%.o: %.c
	$(CXX) $(WARNFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)
