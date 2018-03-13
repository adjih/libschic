/* Cedric Adjih - Inria - 2018 */

#include "msg.h"
#include "shell.h"
#include "xtimer.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#include "buffer.h"


extern void start_udp_sender_thread(char *dst_addr_str, int sleep_delay_sec);
extern void non_auto_init_fakedev_driver(void);

static shell_command_t shell_command_list[] = {
  { NULL, NULL, NULL }
};

/*---------------------------------------------------------------------------*/

#define LETHRYK_MSG_QUEUE_SIZE 16
static msg_t _msg_queue[LETHRYK_MSG_QUEUE_SIZE];

int main(void)
{
    xtimer_sleep(1);

#ifndef MODULE_NETDEV_TAP
    printf("Starting fakedev\n");
    non_auto_init_fakedev_driver();
#else  
    (void)non_auto_init_fakedev_driver;
#endif
    
    start_udp_sender_thread("ff02::1", 1);
  
    msg_init_queue(_msg_queue, LETHRYK_MSG_QUEUE_SIZE);
    
    char line_buffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_command_list, line_buffer, SHELL_DEFAULT_BUFSIZE);
    
    return 0;
}
