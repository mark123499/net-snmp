/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.access_functions.conf,v 1.3 2003/05/31 00:11:57 hardaker Exp $
 */
#ifndef NETSNMPHOSTSTABLE_ACCESS_H
#define NETSNMPHOSTSTABLE_ACCESS_H

/** User-defined data access functions for data in table netSnmpHostsTable */
/** row level accessors */
Netsnmp_First_Data_Point netSnmpHostsTable_get_first_data_point;
Netsnmp_Next_Data_Point netSnmpHostsTable_get_next_data_point;
Netsnmp_Make_Data_Context netSnmpHostsTable_context_convert_function;
Netsnmp_Free_Loop_Context netSnmpHostsTable_loop_free;
Netsnmp_Free_Data_Context netSnmpHostsTable_data_free;

int             netSnmpHostsTable_commit_row(void **my_data_context);

/** column accessors */
long           *get_netSnmpHostAddressType(void *data_context,
                                           size_t * ret_len);
int             set_netSnmpHostAddressType(void *data_context, long *val,
                                           size_t val_len);
char           *get_netSnmpHostAddress(void *data_context,
                                       size_t * ret_len);
int             set_netSnmpHostAddress(void *data_context, char *val,
                                       size_t val_len);
long           *get_netSnmpHostStorage(void *data_context,
                                       size_t * ret_len);
int             set_netSnmpHostStorage(void *data_context, long *val,
                                       size_t val_len);
long           *get_netSnmpHostRowStatus(void *data_context,
                                         size_t * ret_len);
int             set_netSnmpHostRowStatus(void *data_context, long *val,
                                         size_t val_len);

#endif                          /* NETSNMPHOSTSTABLE_ACCESS_H */
