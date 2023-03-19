#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti_NEW/bios_6_50_01_12/packages;C:/ti_NEW/uia_2_20_00_02/packages;C:/ti_NEW/ccsv7/ccs_base
override XDCROOT = C:/ti_NEW/xdctools_3_50_01_12_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti_NEW/bios_6_50_01_12/packages;C:/ti_NEW/uia_2_20_00_02/packages;C:/ti_NEW/ccsv7/ccs_base;C:/ti_NEW/xdctools_3_50_01_12_core/packages;..
HOSTOS = Windows
endif
