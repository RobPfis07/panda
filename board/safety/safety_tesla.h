// board enforces
//   in-state
//      accel set/resume
//   out-state
//      cancel button

// all commands: brake and steering
// if controls_allowed
//     allow all commands up to limit
// else
//     block all commands that produce actuation

void safety_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  // state machine to enter and exit controls (using cruise control lever with a double-pull forward)
  // only turn on openpilot when OEM cruise system is OFF (LED on cruise control lever is OFF)
  // DI_state 0x368 at startbit 12
  /*
  if ((to_push->RIR>>21) == 0x368) {
    // 3 bits starting at position 12
	int cruise_state = (to_push->RDLR & 0xF000) >> 12;
	if (cruise_state != 0x0) {
	  controls_allowed = 0;
	}
  }
  */
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

  // exit controls on brake press
  // DI_torque2::DI_brakePedal 0x118
  /*
  if ((to_push->RIR>>21) == 0x118) {
	// 1 bit at position 16
    if (((to_push->RDLR & 0x8000)) >> 15 == 1) {
      controls_allowed = 0;
    }
  }

  // exit controls on accelerator press
  // DI_torque1::DI_pedalPos 0x108 at startbit 48
  if ((to_push->RIR>>21) == 0x108) {
	// 8 bits starting at position 48 (so use RDHR offset by 16 bits)
    int pedal_position = ((to_push->RDHR & 0xFF0000) >> 16);
    // TODO: what is the threshold to capture accelerator pedal (0-255)
    if (pedal_position > 50) {
      controls_allowed = 0;
    }
  }
  */
}

int safety_tx_hook(CAN_FIFOMailBox_TypeDef *to_send, int hardwired) {
  // BRAKE: safety check
  /*
  if ((to_send->RIR>>21) == 0x1FA) {
    if (controls_allowed) {
      to_send->RDLR &= 0xFFFFFF3F;
    } else {
      to_send->RDLR &= 0xFFFF0000;
    }
  }
  */

  // STEER TODO: safety check
  /*
  if ((to_send->RIR>>21) == 0x488) {
    if (controls_allowed) {
      to_send->RDLR &= 0xFFFFFFFF;
    } else {
      to_send->RDLR &= 0xFF3FFFC0;
    }
  }
  */

  // GAS: safety check
  /*
  if ((to_send->RIR>>21) == 0x200) {
    if (controls_allowed) {
      to_send->RDLR &= 0xFFFFFFFF;
    } else {
      to_send->RDLR &= 0xFFFF0000;
    }
  } 
  */
  // 1 allows the message through
  return hardwired;
}

int safety_tx_lin_hook(int lin_num, uint8_t *data, int len, int hardwired) {
  return hardwired;
}

int flip_bits(int input_bits) {
	unsigned int r = input_bits & 1; // r will be reversed bits of input_bits; first get LSB of input_bits
	int s = sizeof(input_bits) * 8 - 1; // extra shift needed at end

	for (input_bits >>= 1; input_bits; input_bits >>= 1)
	{
	  r <<= 1;
	  r |= input_bits & 1;
	  s--;
	}
	r <<= s; // shift when v's highest bits are zero
	return r;
}

unsigned divu10(unsigned n) {
    unsigned q, r;
    q = (n >> 1) + (n >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = n - (((q << 2) + q) << 1);
    return q + (r > 9);
}
