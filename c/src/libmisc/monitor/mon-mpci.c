/*
 * RTEMS MPCI Config display support
 *
 * TODO
 *
 *  $Id$
 */


#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include "monitor.h"

#include <stdio.h>
#include <stdlib.h>             /* strtoul() */

#define DATACOL 15

/*
 * Fill in entire monitor config table
 * for sending to a remote monitor or printing on the local system
 */

void
rtems_monitor_mpci_canonical(
    rtems_monitor_mpci_t *canonical_mpci,
    void                  *config_void
)
{
    rtems_configuration_table *c = _Configuration_Table;
    rtems_multiprocessing_table *m;
    rtems_mpci_table *mt;

    m = c->User_multiprocessing_table;
    if (m == 0)
        return;
    mt = m->User_mpci_table;
    
    canonical_mpci->node = m->node;
    canonical_mpci->maximum_nodes = m->maximum_nodes;
    canonical_mpci->maximum_global_objects = m->maximum_global_objects;
    canonical_mpci->maximum_proxies = m->maximum_proxies;

    canonical_mpci->default_timeout = mt->default_timeout;
    canonical_mpci->maximum_packet_size = mt->maximum_packet_size;

    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->initialization,
                                            (void *) mt->initialization);

    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->get_packet,
                                            (void *) mt->get_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->return_packet,
                                            (void *) mt->return_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->send_packet,
                                            (void *) mt->send_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->receive_packet,
                                            (void *) mt->receive_packet);
}    

/*
 * This is easy, since there is only 1 (altho we could get them from
 *    other nodes...)
 */

void *
rtems_monitor_mpci_next(
    void                  *object_info,
    rtems_monitor_mpci_t  *canonical_mpci,
    rtems_id              *next_id
)
{
    rtems_configuration_table *c = _Configuration_Table;
    int n = rtems_get_index(*next_id);

    if (n >= 1)
        goto failed;
    
    if ( ! c->User_multiprocessing_table)
        goto failed;
    
    _Thread_Disable_dispatch();

    *next_id += 1;
    return (void *) c;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_mpci_dump_header(
    boolean verbose
)
{
    printf("\
           max     max     max     default    max\n\
   node   nodes  globals  proxies  timeout  pktsize\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
           
    rtems_monitor_separator();
}


void
rtems_monitor_mpci_dump(
    rtems_monitor_mpci_t *monitor_mpci,
    boolean               verbose
)
{
    unsigned32   length = 0;

    length += rtems_monitor_pad(2, length);
    length += printf("  %d", monitor_mpci->node);
    length += rtems_monitor_pad(11, length);
    length += printf("%d", monitor_mpci->maximum_nodes);
    
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->maximum_global_objects);
    
    length += rtems_monitor_pad(28, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->maximum_proxies);

    length += rtems_monitor_pad(37, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->default_timeout);

    length += rtems_monitor_pad(46, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->maximum_packet_size);

    printf("\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += printf("init: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->initialization, verbose);

    printf("\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += printf("get: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->get_packet, verbose);
    length += printf(";  return: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->return_packet, verbose);

    printf("\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += printf("send: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->send_packet, verbose);
    length += printf(";  receive: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->receive_packet, verbose);

    printf("\n");
    length = 0;
}
