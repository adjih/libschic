/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <errno.h>
#include "net/netopt.h"
#include "net/netdev.h"

#include "net/gnrc.h"
#include "net/gnrc/netif/raw.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/*---------------------------------------------------------------------------*/

typedef struct {
    netdev_t netdev;
} fakedev_t;

/* Netdev driver api functions */
static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t netdev_driver_fakedev = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void dump_iovec(const struct iovec *vector, unsigned count)
{
    for (unsigned int i = 0; i < count; i++) {
        uint8_t *data = vector[i].iov_base;
        size_t data_len = vector[i].iov_len;
        for (unsigned j = 0; j < data_len; j++) {
            if (i != 0 || j != 0) {
                if (j == 0) {
                    printf("_");
                }
                else {
                    printf(" ");
                }
            }
            printf("%02x", data[j]);
        }
    }
    printf("\n");
}

static size_t compute_iovec_size(const struct iovec *vector, unsigned count)
{
    size_t result = 0;
    for (unsigned int i = 0; i < count; i++) {
        result += vector[i].iov_len;
    }
    return result;
}

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void)dev;
    (void)vector;
    (void)count;
    DEBUG("fake_driver send %u:\n", count);
    dump_iovec(vector, count);
    size_t result = compute_iovec_size(vector, count);
    return result;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void)dev;
    (void)buf;
    (void)len;
    (void)info;
    DEBUG("fake_driver recv\n");
    return 0;
}

static int _init(netdev_t *netdev)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void)dev;
    DEBUG("fake_driver init\n");
    return 0;
}

static void _isr(netdev_t *netdev)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void) dev;  
    DEBUG("fake_driver isr\n"); 
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void)dev;
    (void)opt;
    (void)value;
    (void)max_len;


    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            DEBUG("fake_driver get NETOPT_DEVICE_TYPE\n");
            assert(max_len == sizeof(uint16_t));
            uint16_t* value_ptr = (uint16_t *)value;
            *value_ptr = NETDEV_TYPE_RAW;
            return sizeof(*value_ptr);

        case NETOPT_ADDRESS:
            DEBUG("fake_driver get NETOPT_ADDRESS\n");
            return -ENOTSUP;
            
        default:
            DEBUG("fake_driver get opt=%d\n", opt);
            return -ENOTSUP;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t len)
{
    fakedev_t *dev = (fakedev_t*) netdev;
    (void)dev;
    (void)opt;
    (void)value;
    (void)len;
    DEBUG("fake_driver set\n");    
    return -ENOTSUP;
}

void fakedev_setup(fakedev_t *dev)
{
    netdev_t *netdev = (netdev_t*) dev;
    netdev->driver = &netdev_driver_fakedev;
}

// XXX: stack sizes of: ../RIOT/sys/auto_init/netif/auto_init_netdev_tap.c 

#define FAKEDEV_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define FAKEDEV_PRIO          (GNRC_NETIF_PRIO)

static char _netdev_fakedev_stack[FAKEDEV_MAC_STACKSIZE];

static fakedev_t fakedev;

void non_auto_init_fakedev_driver(void)
{
    fakedev_setup(&fakedev);
    gnrc_netif_raw_create(_netdev_fakedev_stack,
			  FAKEDEV_MAC_STACKSIZE, FAKEDEV_PRIO,
			  "fakedev", (netdev_t *)&fakedev);
}

/*---------------------------------------------------------------------------*/
