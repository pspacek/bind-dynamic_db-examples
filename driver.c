/**
 * Driver API implementation and main entry point for BIND.
 *
 * BIND calls dynamic_driver_init() during startup
 * and dynamic_driver_destroy() during shutdown.
 *
 * It is completely up to implementation what to do.
 *
 * dynamic-db "name" {} sections in named.conf are independent so driver init()
 * and destroy() functions are called independently for each section even
 * if they reference the same driver/library. It is up to driver implementation
 * to detect and catch this situation if it is undesirable.
 *
 * Copyright (C) 2015  Red Hat ; see COPYING for license
 */
#include <isc/log.h>
#include <isc/task.h>
#include <dns/db.h>
#include <dns/dynamic_db.h>

#define LOG(format, ...) isc_log_write1(dns_lctx, DNS_LOGCATEGORY_DATABASE, \
				       DNS_LOGMODULE_DYNDB, ISC_LOG_INFO,   \
				       "[driver %s][instance %s] " format,  \
				       impname, name, ##__VA_ARGS__)

const char *impname = "minimal-driver";

/**
 * Driver init is is called once during startup and then on every reload.
 *
 * @code
 * dynamic-db "example-name" {
 * 	arg "sample.so";
 * 	arg "param1";
 * 	arg "param2";
 * };
 * @endcode
 * 
 * @param[in] name User-defined string from dynamic-db "name" {}; definition
 *                 in named.conf.
 *                 The example above will have name = "example-name".
 * @param[in] argv User-defined strings from arg parameters in dynamic-db
 *                 definition. The example above will have
 *                 argv[0] = "param1";
 *                 argv[1] = "param2";
 */
isc_result_t
dynamic_driver_init(isc_mem_t *mctx, const char *name, const char * const *argv,
		    dns_dyndb_arguments_t *dyndb_args)
{
	int idx = 0;

	UNUSED(mctx);
	UNUSED(dyndb_args);

	LOG("driver init");
	if (argv != NULL) {
		for (idx = 0; argv[idx] != NULL; idx++) {
			LOG("arg[%d] = '%s'", idx, argv[idx]);
		}
	}

	/* Do whatever you want with variables from dns_dyndb_arguments_t. */

	return ISC_R_SUCCESS;
}

/**
 * Driver destroy is called on every reload and then once during shutdown.
 *
 * @warning
 * It is also called for every dynamic-db section in named.conf but there is no
 * way how to find out for which instance.
 */
void
dynamic_driver_destroy(void)
{
	isc_log_write1(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_DYNDB,
		       ISC_LOG_INFO, "[driver %s] driver destroy",
		       impname);
}
