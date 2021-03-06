/*
 * Copyright (C) 2014--2015  Red Hat ; see COPYING for license
 */

#ifndef LOCK_H_
#define LOCK_H_

#include "instance.h"
#include "util.h"

void
run_exclusive_enter(sample_instance_t *inst, isc_result_t *statep);

void
run_exclusive_exit(sample_instance_t *inst, isc_result_t state);

#endif /* LOCK_H_ */
