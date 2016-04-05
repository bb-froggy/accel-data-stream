#include "worker_src/modules/accel_logging.h"

static uint s_accel_logging_data_sent;

static DataLoggingSessionRef s_logging_session;

static uint s_session_tag;

uint get_accel_logging_data_sent() {
  return s_accel_logging_data_sent;
}

void accel_logging_init() {
  s_accel_logging_data_sent = 0;
  s_session_tag = 0x1402;
}

bool accel_logging_is_busy() {
  return false;
}

void accel_logging_start() {
  s_logging_session = data_logging_create(
    s_session_tag++,            // the unique tag of the logging session
    DATA_LOGGING_BYTE_ARRAY,  // we are logging byte arrays, each of which is
    sizeof(AccelData),        // this length (2+2+2+8=14 bytes) long
    false                     // we do not resume a previous session. They would be overridden.
  ); 
}

DataLoggingResult accel_logging_send_data(AccelData *data, uint32_t num_samples) {
  s_accel_logging_data_sent += num_samples * sizeof(AccelData);
  DataLoggingResult res = data_logging_log(s_logging_session, data, num_samples);
  if (DATA_LOGGING_SUCCESS != res) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "There was a problem logging accel data: %d", res);
    return res;
  }
  else
    return res;
}

void accel_logging_stop() {
  data_logging_finish(s_logging_session);
}