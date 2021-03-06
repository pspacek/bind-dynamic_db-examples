/*
 * Copyright (C) 2009--2015  Red Hat ; see COPYING for license
 */

#include <isc/util.h>

#include <dns/log.h>

#include "log.h"
 
void
log_write(int level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	isc_log_vwrite(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_DYNDB,
		       level, format, args);
	va_end(args);

}
