/*
 *  Chain Handler
 *
 *  NOTE:
 *
 *  The order of this file is to allow proper compilation due to the
 *  order of inlining required by the compiler.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  _Chain_Initialize
 *
 *  This kernel routine initializes a doubly linked chain.
 *
 *  Input parameters:
 *    the_chain        - pointer to chain header
 *    starting_address - starting address of first node
 *    number_nodes     - number of nodes in chain
 *    node_size        - size of node in bytes
 *
 *  Output parameters:  NONE
 */

void _Chain_Initialize(
  Chain_Control *the_chain,
  void           *starting_address,
  unsigned32     number_nodes,
  unsigned32     node_size
)
{
  unsigned32  count;
  Chain_Node *current;
  Chain_Node *next;

  count                     = number_nodes;
  current                   = _Chain_Head( the_chain );
  the_chain->permanent_null = NULL;
  next                      = (Chain_Node *)starting_address;
  while ( count-- ) {
    current->next  = next;
    next->previous = current;
    current        = next;
    next           = (Chain_Node *)
                        _Addresses_Add_offset( (void *) next, node_size );
  }
  current->next    = _Chain_Tail( the_chain );
  the_chain->last  = current;
}

/*PAGE
 *
 *  _Chain_Get_first_unprotected
 */

#ifndef USE_INLINES
Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
)
{
  Chain_Node  *return_node;
  Chain_Node  *new_first;

  return_node         = the_chain->first;
  new_first           = return_node->next;
  the_chain->first    = new_first;
  new_first->previous = _Chain_Head( the_chain );

  return return_node;
}
#endif   /* USE_INLINES */

/*PAGE
 *
 *  _Chain_Get
 *
 *  This kernel routine returns a pointer to a node taken from the
 *  given chain.
 *
 *  Input parameters:
 *    the_chain - pointer to chain header
 *
 *  Output parameters:
 *    return_node - pointer to node in chain allocated
 *    CHAIN_END   - if no nodes available
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

Chain_Node *_Chain_Get(
  Chain_Control *the_chain
)
{
  ISR_Level          level;
  Chain_Node *return_node;

  return_node = NULL;
  _ISR_Disable( level );
    if ( !_Chain_Is_empty( the_chain ) )
      return_node = _Chain_Get_first_unprotected( the_chain );
  _ISR_Enable( level );
  return return_node;
}

/*PAGE
 *
 *  _Chain_Append
 *
 *  This kernel routine puts a node on the end of the specified chain.
 *
 *  Input parameters:
 *    the_chain - pointer to chain header
 *    node      - address of node to put at rear of chain
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Append(
  Chain_Control *the_chain,
  Chain_Node    *node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Append_unprotected( the_chain, node );
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Chain_Extract
 *
 *  This kernel routine deletes the given node from a chain.
 *
 *  Input parameters:
 *    node - pointer to node in chain to be deleted
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Extract(
  Chain_Node *node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Extract_unprotected( node );
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Chain_Insert
 *
 *  This kernel routine inserts a given node after a specified node
 *  a requested chain.
 *
 *  Input parameters:
 *    after_node - pointer to node in chain to be inserted after
 *    node       - pointer to node to be inserted
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Insert_unprotected( after_node, node );
  _ISR_Enable( level );
}
