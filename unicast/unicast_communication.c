#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/packetbuf.h"

#include <string.h>
#include <stdio.h> 

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (8 * CLOCK_SECOND)
static linkaddr_t dest_addr =         {{ 0x00, 0x12, 0x4b, 0x00, 0x11, 0xa7, 0x73, 0x87 }}; //replace this with your receiver's link address


/*---------------------------------------------------------------------------*/
PROCESS(unicast_process, "One to One Communication");
AUTOSTART_PROCESSES(&unicast_process);

/*---------Callback executed immediately after reception---------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest) 
{
  if(len == sizeof(unsigned)) {
    unsigned count;
    memcpy(&count, data, sizeof(count));
    LOG_INFO("Received %u with rssi %d from", count, (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI));
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);

  if(!linkaddr_cmp(&dest_addr, &linkaddr_node_addr)) {
    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      
      char buf[20];
      snprintf(buf, sizeof(buf), "Count %u", count);
      packetbuf_copyfrom(buf, strlen(buf));

      LOG_INFO("Sending unicast to ");
      LOG_INFO_LLADDR(&dest_addr);
      LOG_INFO_("\n");

      unicast_send(&uc, &dest_addr);
      
      count++;
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
