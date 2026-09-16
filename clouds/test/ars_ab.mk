PACKAGES       =  clouds/dsp clouds/dsp/pvoc clouds/test stmlib/utils stmlib/dsp clouds

VPATH          = $(PACKAGES)

TARGET         = ars_ab
BUILD_ROOT     = build/
BUILD_DIR      = $(BUILD_ROOT)$(TARGET)/
CC_FILES       = 		atan.cc \
		ars_ab.cc \
		correlator.cc \
		granular_processor.cc \
		mu_law.cc \
		random.cc \
		resources.cc \
		frame_transformation.cc \
		phase_vocoder.cc \
		stft.cc \
		units.cc
OBJ_FILES      = $(CC_FILES:.cc=.o)
OBJS           = $(patsubst %,$(BUILD_DIR)%,$(OBJ_FILES)) $(STARTUP_OBJ)
DEPS           = $(OBJS:.o=.d)
DEP_FILE       = $(BUILD_DIR)depends.mk

all:  ars_ab

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)%.o: %.cc
	g++ -c -DTEST -g -Wall -Werror -Wno-unused-local-typedefs -I. $< -o $@

$(BUILD_DIR)%.d: %.cc
	g++ -MM -DTEST -I. $< -MF $@ -MT $(@:.d=.o)

ars_ab:  $(OBJS)
	g++ -o $(TARGET) $(OBJS)

depends:  $(DEPS)
	cat $(DEPS) > $(DEP_FILE)

$(DEP_FILE):  $(BUILD_DIR) $(DEPS)
	cat $(DEPS) > $(DEP_FILE)

include $(DEP_FILE)
