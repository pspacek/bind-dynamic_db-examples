/**
 * Driver instance object.
 *
 * One instance is equivalent to dynamic-db section in named.conf.
 * This module parses arguments and provide high-level operations
 * instance init/zone load/instance destroy.
 *
 * Copyright (C) 2008--2015  Red Hat ; see COPYING for license
 */

#include <isc/util.h>

#include <dns/dynamic_db.h>
#include <dns/view.h>
#include <dns/zone.h>

#include "db.h"
#include "util.h"
#include "instance.h"
#include "log.h"
#include "zone.h"

/**
 * Parse parameters and convert them to zone names. Caller has to deallocate
 * resulting DNS names.
 *
 * @param[in]  argv NULL-terminated string array of length 2 (excluding NULL)
 * 		    Each string has to be a valid DNS name.
 * @param[out] z1   Zone name from argv[0]
 * @param[out] z2   Zone name from argv[1]
 */
isc_result_t
parse_params(isc_mem_t *mctx, const char * const *argv, dns_name_t *z1,
	     dns_name_t *z2)
{
	isc_result_t result;
	int idx;

	REQUIRE(argv != NULL);
	REQUIRE(z1 != NULL);
	REQUIRE(z2 != NULL);

	for (idx = 0; argv[idx] != NULL; idx++) {
		log_info("param: '%s'", argv[idx]);
	}
	log_info("number of params: %d", idx);

	if (idx != 2) {
		log_error("exactly two parameters (absolute zone names) are required");
		result = ISC_R_FAILURE;
		goto cleanup;
	}
	CHECK(dns_name_fromstring2(z1, argv[0], dns_rootname, 0, mctx));
	CHECK(dns_name_fromstring2(z2, argv[1], dns_rootname, 0, mctx));

	result = ISC_R_SUCCESS;

cleanup:
	return result;
}

/**
 * Initialize new driver instance. It will not create zones until
 * load_sample_instance_zones() is called.
 */
isc_result_t
new_sample_instance(isc_mem_t *mctx, const char *db_name,
		  const char * const *argv, dns_dyndb_arguments_t *dyndb_args,
		  sample_instance_t **sample_instp)
{
	isc_result_t result;
	sample_instance_t *inst = NULL;

	log_info("creating sample dyndb instance '%s'", db_name);

	REQUIRE(sample_instp != NULL && *sample_instp == NULL);

	CHECKED_MEM_GET_PTR(mctx, inst);
	ZERO_PTR(inst);
	isc_mem_attach(mctx, &inst->mctx);
	dns_name_init(&inst->zone1_name, NULL);
	dns_name_init(&inst->zone2_name, NULL);

	CHECK(parse_params(mctx, argv, &inst->zone1_name, &inst->zone2_name));

	inst->db_name = db_name;
	inst->view = dns_dyndb_get_view(dyndb_args);
	inst->zmgr = dns_dyndb_get_zonemgr(dyndb_args);
	inst->task = dns_dyndb_get_task(dyndb_args);

	*sample_instp = inst;
	result = ISC_R_SUCCESS;

cleanup:
	if (result != ISC_R_SUCCESS)
		destroy_sample_instance(&inst);
	return result;
}

/**
 * Create empty zones, add fake SOA, NS, and A records, load fake zones
 * and add them to inst->view.
 */
isc_result_t
load_sample_instance_zones(sample_instance_t *inst)
{
	isc_result_t result;

	CHECK(create_zone(inst, &inst->zone1_name, &inst->zone1));
	CHECK(activate_zone(inst, inst->zone1));

	CHECK(create_zone(inst, &inst->zone2_name, &inst->zone2));
	CHECK(activate_zone(inst, inst->zone2));

cleanup:
	return result;
}

void
destroy_sample_instance(sample_instance_t **instp)
{
	sample_instance_t *inst;
	const char *db_name;
	REQUIRE(instp != NULL);

	inst = *instp;
	if (inst == NULL)
		return;

	db_name = inst->db_name; /* points to DB instance: outside inst */
	log_info("destroying sample dyndb instance '%s'", db_name);

	if (inst->zone1 != NULL)
		dns_zone_detach(&inst->zone1);
	if (inst->zone2 != NULL)
		dns_zone_detach(&inst->zone2);

	dns_name_free(&inst->zone1_name, inst->mctx);
	dns_name_free(&inst->zone2_name, inst->mctx);

	MEM_PUT_AND_DETACH(inst);

	*instp = NULL;
	log_info("sample dyndb instance '%s' destroyed", db_name);
}
