void default_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  UNUSED(to_push);
}

int block = 0;
uint32_t eon_detected_last = 0;
void send_spoof_292(void);
void send_spoof_396(void);
void send_spoof_43A(void);
void send_spoof_43B(void);
void send_spoof_497(void);
void send_spoof_4CC(void);
uint32_t startedtime = 0;
bool onboot = 0;
bool boot_done = 0;
void send_id(void);

// *** no output safety mode ***

static void nooutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = 0;
}

static int nooutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return false;
}

static int nooutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return false;
}
uint8_t button_state = 0;
// TODO: make this only happen on Toyotas. write some kind of detection
static int default_fwd_hook(int bus_num, CAN_FIFOMailBox_TypeDef *to_fwd) {
  // UNUSED(bus_num);
  // UNUSED(to_fwd);
  int bus_fwd = -1;
  if(bus_num == 2) {
      send_spoof_292();
    
      send_spoof_396();
    
      send_spoof_43A();
      send_spoof_43B();
      send_spoof_497();
      send_spoof_4CC();
  }
  return bus_fwd;
}

const safety_hooks nooutput_hooks = {
  .init = nooutput_init,
  .rx = default_rx_hook,
  .tx = nooutput_tx_hook,
  .tx_lin = nooutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};

// *** all output safety mode ***

static void alloutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = 1;
}

static int alloutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return true;
}

static int alloutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return true;
}

const safety_hooks alloutput_hooks = {
  .init = alloutput_init,
  .rx = default_rx_hook,
  .tx = alloutput_tx_hook,
  .tx_lin = alloutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};
