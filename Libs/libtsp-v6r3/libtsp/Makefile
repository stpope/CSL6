#
# run make in subdirectories
#
# $Id: Makefile 1.1 1995/01/11 libtsp-v6r3 $

SHELL=/bin/sh

# The default target is treated as a special case
default_target=__DEFAULT_TARGET__
.PHONY:	$(default_target)
$(default_target):
	@for file in */[Mm]akefile; do \
		if test "$$file" = "*/[Mm]akefile"; then \
			echo "<<< No makefiles found >>>"; \
			continue; \
		fi; \
		dir=`dirname $$file`; \
		mf=`basename $$file`; \
		(cd $$dir; $(MAKE) -f $$mf); \
	done

# Named targets
.DEFAULT:
	@for file in */[Mm]akefile; do \
		if test "$$file" = "*/[Mm]akefile"; then \
			echo "<<< No makefiles found >>>"; \
			continue; \
		fi; \
		dir=`dirname $$file`; \
		mf=`basename $$file`; \
		(cd $$dir; $(MAKE) -f $$mf $@); \
	done
