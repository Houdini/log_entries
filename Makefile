MODULES = log_entries
EXTENSION = log_entries
DATA = log_entries--1.0.sql

LDFLAGS_SL += -L$(top_builddir)/src/port -lpgport

REGRESS = log_entries

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/log_entries
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
