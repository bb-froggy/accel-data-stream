#include <pebble_worker.h>

#include "worker_src/modules/accel_logging.h"

#define SAMPLES_PER_UPDATE 20

static bool s_sending;
static int s_send_counter;

static void update_data_sent() {
  uint16_t data_sent_current = get_accel_logging_data_sent() / 1000;
  static uint16_t data_sent_last = -1;
  if (data_sent_last == data_sent_current)
    return;  // info is up-to-date enough
  data_sent_last = data_sent_current;

    // Construct a data packet
  AppWorkerMessage message = {
    .data0 = data_sent_current
  };
 
  app_worker_send_message(1, &message); // 1 = number of kb sent
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  DataLoggingResult res = accel_logging_send_data(data, num_samples);
  if (DATA_LOGGING_SUCCESS == res) {
    s_send_counter++;
    update_data_sent();    
  }
  else {
      // Construct a data packet
    AppWorkerMessage message = {
      .data0 = res
    };
   
    app_worker_send_message(2, &message); // 2 = logging error
  }
}

static void begin_sending_data() {
  if(s_sending)
    return;  // has already begun

  // Begin sending data
  s_sending = true;

  accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  accel_data_service_subscribe(SAMPLES_PER_UPDATE, accel_data_handler);

  accel_logging_start();
}

static void stop_sending_data() {
  if(!s_sending)
    return;  // does not send currently

  // Stop sending data
  s_sending = false;

  accel_data_service_unsubscribe();

  accel_logging_stop();
}

static void init() {
  s_send_counter = 0;
  begin_sending_data();
}

static void deinit() {
  stop_sending_data();
}

int main(void) {
  init();
  worker_event_loop();
  deinit();
}