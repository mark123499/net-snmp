/*
 * snmpbulkwalk.c - send SNMPv2 Bulk requests to a network entity, walking a
 * subtree.
 *
 */
/*********************************************************************
	Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
**********************************************************************/
#include <config.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRINGS_H
#include <strings.h>
#else
#include <string.h>
#endif
#include <sys/types.h>
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <stdio.h>
#if HAVE_WINSOCK_H
#include <winsock.h>
#else
#include <netdb.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "asn1.h"
#include "snmp_api.h"
#include "snmp_client.h"
#include "mib.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "party.h"
#include "context.h"
#include "view.h"
#include "acl.h"
#include "system.h"
#include "snmp_parse_args.h"

void main __P((int, char **));

oid objid_mib[] = {1, 3, 6, 1, 2, 1};

void
usage __P((void))
{
  fprintf(stderr,"Usage:\n  snmpbulkwalk ");
  snmp_parse_args_usage(stderr);
  fprintf(stderr," [<objectID>]\n\n");
  snmp_parse_args_descriptions(stderr);
}

void
main(argc, argv)
    int   argc;
    char  *argv[];
{
    struct snmp_session  session, *ss;
    struct snmp_pdu *pdu;
    struct snmp_pdu *response;
    struct variable_list *vars;
    int  arg;
    oid  name[MAX_NAME_LEN];
    int  name_length;
    oid  root[MAX_NAME_LEN];
    int  rootlen;
    int  count;
    int  running;
    int  status;
    int  reps = 1000;

    /* read in MIB database */
    init_mib();

    /* get the common command line arguments */
    arg = snmp_parse_args(argc, argv, &session);

    /* get the initial object and subtree */
    if (arg < argc) {
      /* specified on the command line */
      rootlen = MAX_NAME_LEN;
      if (!read_objid(argv[arg], root, &rootlen)){
        printf("Invalid object identifier: %s\n", argv[arg]);
        exit(1);
      }
    } else {
      /* use default value */
      memmove(root, objid_mib, sizeof(objid_mib));
      rootlen = sizeof(objid_mib) / sizeof(oid);
    }

    SOCK_STARTUP;

    /* open an SNMP session */
    snmp_synch_setup(&session);
    ss = snmp_open(&session);
    if (ss == NULL){
      snmp_perror("snmpbulkwalk: Couldn't open snmp");
      SOCK_CLEANUP;
      exit(1);
    }

    /* setup initial object name */
    memmove(name, root, rootlen * sizeof(oid));
    name_length = rootlen;

    running = 1;
    while(running) {
      /* create PDU for GETBULK request and add object name to request */
      pdu = snmp_pdu_create(BULK_REQ_MSG);
      pdu->non_repeaters = 0;
      pdu->max_repetitions = reps;  /* fill the packet */
      snmp_add_null_var(pdu, name, name_length);

      /* do the request */
      status = snmp_synch_response(ss, pdu, &response);
      if (status == STAT_SUCCESS){
        if (response->errstat == SNMP_ERR_NOERROR){
          /* check resulting variables */
          for(vars = response->variables; vars; vars = vars->next_variable){
            if ((vars->name_length < rootlen) ||
                (memcmp(root, vars->name, rootlen * sizeof(oid))!=0)){
               /* not part of this subtree */  
              running = 0;
              continue;
            }
            print_variable(vars->name, vars->name_length, vars);
            if ((vars->type == SNMP_ENDOFMIBVIEW) ||
                (vars->type == SNMP_NOSUCHOBJECT) ||
                (vars->type == SNMP_NOSUCHINSTANCE)){
              /* an exception value */
              running = 0;
            }
            /* check if last variable, and if so, save for next request */
            if (!vars->next_variable){
              memmove(name, vars->name, vars->name_length * sizeof(oid));
              name_length = vars->name_length;
            }
          }
        } else {
          /* error in response, print it */
          running = 0;
          if (response->errstat == SNMP_ERR_NOSUCHNAME){
            printf("End of MIB.\n");
          } else {
            printf("Error in packet.\nReason: %s\n",
                   snmp_errstring(response->errstat));
            if (response->errstat == SNMP_ERR_NOSUCHNAME){
              printf("The request for this object identifier failed: ");
              for(count = 1, vars = response->variables;
                    vars && (count != response->errindex);
                    vars = vars->next_variable, count++)
                /*EMPTY*/;
              if (vars)
                print_objid(vars->name, vars->name_length);
              printf("\n");
            }
          }
        }
      } else if (status == STAT_TIMEOUT){
        printf("No Response from %s\n", session.peername);
        running = 0;
      } else {    /* status == STAT_ERROR */
        snmp_perror("snmpbulkwalk: An error occurred: ");
        running = 0;
      }

      if (response)
        snmp_free_pdu(response);
    }

    snmp_close(ss);
    SOCK_CLEANUP;
    exit (0);
}
