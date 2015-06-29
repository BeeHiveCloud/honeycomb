#ifndef INCLUDE_git_mysql_config_h__
#define INCLUDE_git_mysql_config_h__

#include "git2.h"
#include "git2/sys/config.h"
#include "mysql_backend.h"

typedef struct {
	git_config_backend parent;
	git_mysql *mysql;
} git_mysql_config;

int git_mysql_config_init(git_config_backend **out, git_mysql *mysql);

#endif
