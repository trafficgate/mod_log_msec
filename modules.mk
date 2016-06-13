mod_log_msec.la: mod_log_msec.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_log_msec.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_log_msec.la
