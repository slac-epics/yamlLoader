TOP=../..

include $(TOP)/configure/CONFIG

USR_CXXFLAGS += -std=c++0x
USR_CXXFLAGS += -DHAVE_YAML

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

INCLUDES += -I$(YAML_INCLUDE)
INCLUDES += -I$(BOOST_INCLUDE)

## this part is just for yaml loader driver
INCLUDES += -I$(CPSW_FRAMEWORK_INCLUDE)

INC += yamlLoader.h
DBD += yamlLoader.dbd


LIBRARY_IOC = yamlLoader

yamlLoader_DBD += yamlLoader.dbd
yamlLoader_SRCS += yamlLoader.cpp



yaml-cpp_DIR += $(YAML_LIB)
yamlLoader_LIBS += yaml-cpp

cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
yamlLoader_LIBS += cpsw


yamlLoader_LIBS += $(EPICS_BASE_IOC_LIBS)





#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
#
