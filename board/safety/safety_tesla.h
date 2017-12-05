// board enforces
//   in-state
//      accel set/resume
//   out-state
//      cancel button
//      accel rising edge
//      brake rising edge
//      brake > 0mph

static void tesla_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  if ((to_push->RIR>>21) == 0x45) {
    // 6 bits starting at position 0
    int lever_position = (to_push->RDLR & 0x3F);
  if (lever_position == 1) { // pull forward
    // activate openpilot
    //if (current_car_time <= time_at_last_stalk_pull + 1 && current_car_time != -1 && time_at_last_stalk_pull != -1) {
    controls_allowed = 1;
    //}
    time_at_last_stalk_pull = current_car_time;
  } else if (lever_position == 2) { // push backward
    // deactivate openpilot
    controls_allowed = 0;
  }
}

// all commands: gas, brake and steering
// if controls_allowed and no pedals pressed
//     allow all commands up to limit
// else
//     block all commands that produce actuation

static int tesla_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  // TODO: add safety
  return true;
}

static int tesla_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  // TODO: add safety if using LIN
  return true;
}

static void tesla_init() {
  controls_allowed = 0;
}

const safety_hooks tesla_hooks = {
  .init = tesla_init,
  .rx = tesla_rx_hook,
  .tx = tesla_tx_hook,
  .tx_lin = tesla_tx_lin_hook,
};


