#ifndef INCLUDE_git_pq_backend_h__
#define INCLUDE_git_pq_backend_h__

#include "git2.h"
#include <libpq-fe.h>

GIT_BEGIN_DECL

GIT_EXTERN(int)git_pq_init(PGconn **out, const char *conninfo);

GIT_END_DECL

#endif
