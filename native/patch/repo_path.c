#include "repo_path.h"

#include "../../vendor/libgit2/src/common.h"
#include "../../vendor/libgit2/src/repository.h"


int git_repository_set_path(git_repository *repo, const char *path)
{
	int error = 0;
	git_buf buf = GIT_BUF_INIT;

	assert(repo && path);

	if (git_path_prettify_dir(&buf, path, NULL) < 0)
		return -1;

	if (repo->path_repository && strcmp(repo->path_repository, buf.ptr) == 0)
		return 0;
	
	if (!error) {
		char *old_path = repo->path_repository;
		repo->path_repository = git_buf_detach(&buf);
		git__free(old_path);
	}

	return error;
}
