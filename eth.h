#include <stdbool.h>

extern void eth_init(void);

extern uint8_t * eth_get_tx_buf(void);
extern void eth_send(uint32_t length);

extern uint8_t * eth_receive(uint32_t * length);
extern void eth_release_rx_buf(void);

extern bool eth_get_link_status(void);
