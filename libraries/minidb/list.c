/* mtest.c - memory-mapped database tester/toy */
/*
 * Copyright 2011 Howard Chu, Symas Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
#define _XOPEN_SOURCE 500    /* srandom(), random() */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lmdb.h"

#define ERROUT(rc) ({printf("err=[%d] %s\n",(rc),mdb_strerror((rc))); return rc;})

int hexprint(int l, char *str) {
 int i;
 for(i=0; i<l; i++) {
   printf("%x",str[i]);
 }
 printf("\n");
}

int main(int argc,char * argv[])
{
  int rc;
  MDB_env *env;
  MDB_dbi dbi;
  MDB_val key, data;
  MDB_txn *txn;
  MDB_stat mst;
  MDB_cursor *cursor;
  if(argc<1) { printf("Usage: list <dbpath>\n"); return 1;}

  rc = mdb_env_create(&env);
  if(rc) ERROUT(rc);
  rc = mdb_env_set_mapsize(env, 10485760); //LMDB_MAXSIZE
  if(rc) ERROUT(rc);
  rc = mdb_env_open(env, argv[1], MDB_FIXEDMAP/*MDB_SUBDIR |MDB_NOSYNC*/, 0664); //For read: MDB_NOSUBDIR
  if(rc) ERROUT(rc);
  rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn); //read = MDB_RDONLY
  if(rc) ERROUT(rc);
  //rc = mdb_env_stat(env, &mst);
  rc = mdb_cursor_open(txn, dbi, &cursor);
  if(rc) ERROUT(rc);
  printf("Reading store:\n");
  while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
    printf(">key: %d, data: %d\n",
      (int) key.mv_size,
      (int) data.mv_size);
   hexprint((int) key.mv_size, (char *) key.mv_data);
   hexprint((int) data.mv_size, (char *) data.mv_data);
/*
    printf("key: %.*s, data: %.*s\n",
      (int) key.mv_size,  (char *) key.mv_data,
      (int) data.mv_size, (char *) data.mv_data);
*/  
  }
  mdb_txn_abort(txn);
  mdb_cursor_close(cursor);
  mdb_close(env, dbi);

  mdb_env_close(env);

  return 0;
}
