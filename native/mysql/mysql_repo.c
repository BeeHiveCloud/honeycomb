#include "mysql_repo.h"

int git_repo_set_dummy_path(git_repository *repo){

	int error;
	
	//repo->path_repository = "/.git";
	error = git_repository_set_workdir(repo, "/", 0);

	return error;
}