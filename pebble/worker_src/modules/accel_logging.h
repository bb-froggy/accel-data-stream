#pragma once

#include <pebble_worker.h>

void accel_logging_init();

void accel_logging_start();

bool accel_logging_is_busy();

DataLoggingResult accel_logging_send_data(AccelData *data, uint32_t num_samples);

void accel_logging_stop();

uint get_accel_logging_data_sent();