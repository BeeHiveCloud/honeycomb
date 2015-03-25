#include "mysql_repo.h"

#include "../../libgit2/src/common.h"
#include "../../libgit2/src/thread-utils.h"
#include "../../libgit2/src/repository.h"

#include <string.h>

int git_repository_set_path(git_repository *repo, char *path){

	assert(repo && path);

	repo->path_repository = path;

	return 0;
}