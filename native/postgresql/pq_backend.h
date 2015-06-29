#ifndef INCLUDE_git_pq_backend_h__
#define INCLUDE_git_pq_backend_h__

#include <libpq-fe.h>

int git_pq_init(PGconn **out, const char *conninfo);

int git_pq_free(PGconn *conn);

#endif
