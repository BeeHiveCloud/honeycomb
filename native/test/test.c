#include <stdio.h>

#include <git2/sys/repository.h>

#include "../mysql/mysql_backend.h"
#include "../mysql/mysql_index.h"
#include "../mysql/mysql_odb.h"
#include "../mysql/mysql_refdb.h"
#include "../mysql/mysql_repo.h"
#include "../mysql/mysql_tree.h"
#include "../patch/repo_path.h"

git_mysql *mysql;
git_repository *repo;

void create(char *name, char *desc){
  int error = GIT_ERROR;
  my_ulonglong rid = 0;

  printf("name:%s,desc%s\n", name, desc);

  git_mysql_transaction(mysql);

  rid = git_mysql_repo_create(mysql, 1, name, desc);
  if (rid > 0){
    mysql->repo = rid;
    printf("rid:%lld\n",mysql->repo);
    git_oid oid;
    git_reference *ref;
    git_tree *tree;
    git_signature *me;
    git_oid commit;
    error = git_reference_symbolic_create(&ref, repo, "HEAD", "refs/heads/master", 0, NULL, NULL);
    git_reference_free(ref);

    error = git_blob_create_frombuffer(&oid, repo, name, strlen(name));
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_blob_create_frombuffer error\n");
    }

    error = git_mysql_index_write(mysql, &oid, "/README.md");
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_mysql_index_write error\n");
    }

    error = git_mysql_tree_init(mysql);
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_mysql_tree_init error\n");
    }

    error = git_mysql_tree_build(mysql, repo, "BLOB");
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_mysql_tree_build error\n");
    }

    error = git_mysql_tree_build(mysql, repo, "TREE");
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_mysql_tree_build error\n");
    }

    tree = git_mysql_tree_root(mysql, repo);
    if (!tree){
      git_mysql_rollback(mysql);
      printf("git_mysql_tree_build error\n");
    }

    const git_oid *tid = git_tree_id(tree);
	char sha1[GIT_OID_HEXSZ + 1] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, tid);
    printf("tree oid:%s\n",sha1);

    error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
    if (error < 0){
      printf("git_signature_now error\n");
    }

    error = git_commit_create(&commit, repo, "refs/heads/master", me, me, "UTF-8", "Initial Commit", tree, 0, NULL);
    if (error < 0){
      git_mysql_rollback(mysql);
      printf("git_commit_create error\n");
    }

    git_mysql_commit(mysql);
    git_signature_free(me);
    git_tree_free(tree);
  }
  else{
    git_mysql_rollback(mysql);
    printf("git_mysql_repo_create error\n");
  }

}

void add(int rid, char *path, char *content){
  int error = GIT_ERROR;
  git_oid oid;

  printf("path:%s,content:%s\n", path, content);

  git_mysql_transaction(mysql);

  mysql->repo = rid;

  error = git_blob_create_frombuffer(&oid, repo, content, strlen(content));
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_blob_create_frombuffer error\n");
  }

  error = git_mysql_index_write(mysql, &oid, path);
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_mysql_index_write error\n");
  }

  git_mysql_commit(mysql);
}

void commit(int rid, char *message){
  int error = GIT_ERROR;
  git_signature *me;
  git_oid commit;
  git_tree *tree;
  git_object *parent;

  printf("message:%s\n", message);

  mysql->repo = rid;

  error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
  if (error < 0){
    printf("git_signature_now error\n");
  }

  error = git_revparse_single(&parent, repo, "HEAD^{commit}");
  if (error < 0){
    printf("git_revparse_single error\n");
  }

  // Transaction Start
  git_mysql_transaction(mysql);

  error = git_mysql_tree_init(mysql);
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_mysql_tree_init error\n");
  }

  error = git_mysql_tree_build(mysql, repo, "BLOB");
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_mysql_tree_build error\n");
  }
  error = git_mysql_tree_build(mysql, repo, "TREE");
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_mysql_tree_build error\n");
  }
  tree = git_mysql_tree_root(mysql, repo);
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_mysql_tree_build error\n");
  }

  const git_commit *parents[] = { (git_commit *)parent };
  error = git_commit_create(&commit, repo, "HEAD", me, me, "UTF-8", message, tree, 1, parents);
  if (error < 0){
    git_mysql_rollback(mysql);
    printf("git_commit_create error\n");
  }

  git_mysql_commit(mysql);

  git_signature_free(me);
}

void diff(int rid){
  int error = GIT_ERROR;
  git_object *commit;
  git_commit *parent = NULL;

  mysql->repo = rid;

  error = git_revparse_single(&commit, repo, "HEAD^{commit}");
  error = git_commit_parent(&parent, (git_commit *)commit, 0);

  git_tree *commit_tree = NULL, *parent_tree = NULL;
  error = git_commit_tree(&commit_tree, (git_commit *)commit);
  error = git_commit_tree(&parent_tree, parent);

  git_diff *diff = NULL;
  git_diff_options opts;
  error = git_diff_init_options(&opts, GIT_DIFF_OPTIONS_VERSION);

  error = git_diff_tree_to_tree(&diff, repo, commit_tree, parent_tree, &opts);
	if (error < 0){
		printf("git_diff_tree_to_tree error\n");
	}

	git_mysql_tree_diff(diff);
	if (error < 0){
		printf("git_mysql_tree_diff error\n");
	}

	git_object_free(commit);
	git_commit_free(parent);
	git_tree_free(commit_tree);
	git_tree_free(parent_tree);
	git_diff_free(diff);

}



int main(int argc, char **argv){
  int error = GIT_ERROR;

  printf("Usage: \n");
  printf("  create <repo> <desc>               :create a new repo, return repo id\n");
  printf("  add    <rid>  <path>    <content>  :add blob to current repo\n");
  printf("  commit <rid>  <message>            :commit\n");
  printf("  diff   <rid>                       :diff\n\n");

  error = git_mysql_init(&mysql,"localhost","git","git","git",0,NULL,0);
  if (error < 0){
	  printf("git_mysql_init error \n");
  }
  error = git_libgit2_init();
  if (error < 0){
    printf("git_libgit2_init error \n");
  }
  git_odb_backend   *odb_backend;
  error = git_mysql_odb_init(&odb_backend, mysql);
  if (error < 0){
    printf("git_mysql_odb_init error \n");
  }
  error = git_odb_new(&odb_backend->odb);
  if (error < 0){
    printf("git_odb_new error \n");
  }
  error = git_odb_add_backend(odb_backend->odb, odb_backend, 1);
  if (error < 0){
    printf("git_odb_add_backend error \n");
  }
  error = git_repository_wrap_odb(&repo, odb_backend->odb);
  if (error < 0){
    printf("git_repository_wrap_odb error \n");
  }
  git_refdb *refdb;
  error = git_refdb_new(&refdb,repo);
  if (error < 0){
    printf("git_refdb_new error \n");
  }
  git_refdb_backend   *refdb_backend;
  error = git_mysql_refdb_init(&refdb_backend, mysql);
  if (error < 0){
    printf("git_mysql_refdb_init error \n");
  }
  error = git_refdb_set_backend(refdb, refdb_backend);
  if (error < 0){
    printf("git_refdb_set_backend error \n");
  }
  git_repository_set_refdb(repo, refdb);
  git_config *cfg;
  error = git_config_open_default(&cfg);
  if (error < 0){
    printf("git_config_open_default error \n");
  }
  git_repository_set_config(repo,cfg);
  error = git_repository_set_workdir(repo, "/", 0);
  if (error < 0){
    printf("git_repository_set_workdir error \n");
  }
  error = git_repository_set_path(repo, "/");
  if (error < 0){
    printf("git_repository_set_path error \n");
  }

  if(argc < 2)
    return 0; /*do nothing if no arguments passed*/
  else{
    char *cmd = argv[1];

    if(!strcmp(cmd,"create"))
      create(argv[2], argv[3]);
    else if(!strcmp(cmd,"add"))
      add(strtol(argv[2], (char **)NULL, 10), argv[3], argv[4]);
    else if(!strcmp(cmd,"commit"))
      commit(strtol(argv[2], (char **)NULL, 10), argv[3]);
    else if(!strcmp(cmd,"diff"))
      diff(strtol(argv[2], (char **)NULL, 10));
    else
      printf("wrong cmd\n");
  }

  error = git_mysql_free(mysql);
  if (error < 0){
    printf("git_mysql_free error \n");
  }
  error = git_libgit2_shutdown();
  if (error < 0){
    printf("git_libgit2_shutdown error \n");
  }

  return 0;
}
