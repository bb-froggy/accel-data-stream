#include "comm.h"

/*
static bool s_busy;

bool comm_is_busy() {
  return s_busy;
}

static void out_failed_handler(DictionaryIterator *iter, AppMessageResult result, void *context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "out_failed_handler %d", (int)result);
}

static void outbox_sent_handler(DictionaryIterator *iterator, void *context) {
  s_busy = false;
}
*/

static ActionTrigger startCallback;
static ActionTrigger stopCallback;

void registerStartCallback(ActionTrigger newStartCallback) {
  startCallback = newStartCallback;
}

void registerStopCallback(ActionTrigger newStopCallback) {
  stopCallback = newStopCallback;
}

void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  Tuple *firstData = dict_read_first(iterator);
  if (TUPLE_UINT != firstData->type) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "received a non uint message!");
    return;
  }
  
  if (6001 == firstData->value->uint32 && NULL != startCallback)
    (*startCallback)();
  else if (6002 == firstData->value->uint32 && NULL != stopCallback)
    (*stopCallback)();
  else {
      APP_LOG(APP_LOG_LEVEL_WARNING, "received a message with unknown id: %lu", firstData->value->uint32);  
  }
}

void comm_init() {
  startCallback = NULL;
  stopCallback = NULL;
//  app_message_register_outbox_sent(outbox_sent_handler);
//  app_message_register_outbox_failed(out_failed_handler);
//  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(COMM_SIZE_INBOX, COMM_SIZE_OUTBOX);
}
