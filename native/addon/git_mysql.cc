#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "git_mysql.h"

void GitMysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

    Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "Open", Open);

	NODE_SET_METHOD(object, "Close", Close);

	NODE_SET_METHOD(object, "Add", CreateBlob);

	NODE_SET_METHOD(object, "WriteTree", WriteTree);

	NODE_SET_METHOD(object, "BranchLookup", BranchLookup);

	NODE_SET_METHOD(object, "CreateRef", CreateRef);

	NODE_SET_METHOD(object, "RevParse", RevParse);

	NODE_SET_METHOD(object, "Commit", Commit);

	NODE_SET_METHOD(object, "CreateBranch", CreateBranch);

	NODE_SET_METHOD(object, "AdHoc", AdHoc);

	NODE_SET_METHOD(object, "CreateRepo", CreateRepo);

	NODE_SET_METHOD(object, "DeleteRepo", DeleteRepo);

	NODE_SET_METHOD(object, "GetRepo", GetRepo);

	NODE_SET_METHOD(object, "SetRepo", SetRepo);

	NODE_SET_METHOD(object, "TreeWalk", TreeWalk);

	NODE_SET_METHOD(object, "Config", Config);

    target->Set(NanNew<String>("MySQL"), object);
}


NAN_METHOD(GitMysql::Open) {
  NanEscapableScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("mysql host is required.");
  }

  if (args.Length() == 1 || !args[1]->IsString()) {
    return NanThrowError("mysql user is required.");
  }

  if (args.Length() == 2 || !args[2]->IsString()) {
    return NanThrowError("mysql password is required.");
  }

  if (args.Length() == 3 || !args[3]->IsString()) {
    return NanThrowError("mysql db is required.");
  }

  if (args.Length() == 4 || !args[4]->IsNumber()) {
    return NanThrowError("mysql port required.");
  }

// start convert_from_v8 block
  const char * from_mysql_host;
  String::Utf8Value mysql_host(args[0]->ToString());
  from_mysql_host = (const char *) strdup(*mysql_host);

  const char * from_mysql_user;
  String::Utf8Value mysql_user(args[1]->ToString());
  from_mysql_user = (const char *) strdup(*mysql_user);

  const char * from_mysql_passwd;
  String::Utf8Value mysql_passwd(args[2]->ToString());
  from_mysql_passwd = (const char *) strdup(*mysql_passwd);

  const char * from_mysql_db;
  String::Utf8Value mysql_db(args[3]->ToString());
  from_mysql_db = (const char *) strdup(*mysql_db);

  unsigned int from_mysql_port;
  from_mysql_port = (unsigned int)args[4]->ToNumber()->Value();
// end convert_from_v8 block

  int error = git_mysql_init(&mysql,
							  from_mysql_host,
							  from_mysql_user,
							  from_mysql_passwd,
							  from_mysql_db,
							  from_mysql_port,
							  NULL,
							  0);

  if (error < 0){
	  return NanThrowError("git_mysql_init error");
  }

  error = git_libgit2_init();
  if (error < 0){
	  return NanThrowError("git_libgit2_init error");
  }

  git_odb_backend   *odb_backend;
  error = git_mysql_odb_init(&odb_backend, mysql);
  if (error < 0){
	  return NanThrowError("git_mysql_odb_init error");
  }

  error = git_odb_new(&odb_backend->odb);
  if (error < 0){
	  return NanThrowError("git_odb_new error");
  }

  error = git_odb_add_backend(odb_backend->odb, odb_backend, 1);
  if (error < 0){
	  return NanThrowError("git_odb_add_backend error");
  }

  error = git_repository_wrap_odb(&repo, odb_backend->odb);
  if (error < 0){
	  return NanThrowError("git_repository_wrap_odb error");
  }

  git_refdb *refdb;
  error = git_refdb_new(&refdb,repo);
  if (error < 0){
	  return NanThrowError("git_refdb_new error");
  }

  git_refdb_backend   *refdb_backend;
  error = git_mysql_refdb_init(&refdb_backend, mysql);
  if (error < 0){
	  return NanThrowError("git_mysql_refdb_init error");
  }

  error = git_refdb_set_backend(refdb, refdb_backend);
  if (error < 0){
	  return NanThrowError("git_mysql_refdb_init error");
  }

  git_repository_set_refdb(repo, refdb);

  git_config *cfg;
  error = git_config_open_default(&cfg);
  if (error < 0){
	  return NanThrowError("git_config_open_default error");
  }
  
  /*
  git_config_backend *cfg_backend;

  error = git_mysql_config_init(&cfg_backend, mysql);
  if (error < 0){
	  return NanThrowError("git_mysql_config_init error");
  }

  error = git_config_new(&cfg);
  if (error < 0){
	  return NanThrowError("git_config_new error");
  }

  error = git_config_add_backend(cfg, cfg_backend, GIT_CONFIG_LEVEL_LOCAL, 0);
  if (error < 0){
	  return NanThrowError("git_config_add_backend error");
  }
  */

  git_repository_set_config(repo,cfg);


  error = git_repository_set_workdir(repo, "/", 0);
  if (error < 0){
	  return NanThrowError("git_repository_set_workdir error");
  }

  error = git_repository_set_path(repo, "/");
  if (error < 0){
	  return NanThrowError("git_repository_set_path error");
  }

  if (!error)
	  NanReturnValue(NanTrue());
  else
	  NanReturnValue(NanFalse());

}


NAN_METHOD(GitMysql::Close) {
	NanEscapableScope();

	int error = git_mysql_free(mysql);

	error = git_libgit2_shutdown();
	if (error < 0){
		return NanThrowError("git_libgit2_shutdown error");
	}

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::CreateBlob) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("path is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("buffer is required.");
	}

	// start convert_from_v8 block
	const char *from_path;
	String::Utf8Value path_buf(args[0]->ToString());
	from_path = (const char *)strdup(*path_buf);

	const char *from_buf;
	String::Utf8Value blob_buf(args[1]->ToString());
	from_buf = (const char *)strdup(*blob_buf);
	// end convert_from_v8 block

	int				  error;
	git_oid			  oid;

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_blob_create_frombuffer(&oid, repo, from_buf, strlen(from_buf));
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_blob_create_frombuffer error");
	}

	error = git_mysql_index_add(mysql, &oid, from_path);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_index_write error");
	}

	git_mysql_commit(mysql);

	char sha1[GIT_OID_HEXSZ+1] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ+1, &oid);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);
}

NAN_METHOD(GitMysql::WriteTree) {
	NanEscapableScope();

	int error;
	git_tree *tree;

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_mysql_tree_init(mysql);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_tree_init error");
	}

	error = git_mysql_tree_build(mysql, repo, "BLOB");
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_tree_build error");
	}
	error = git_mysql_tree_build(mysql, repo, "TREE");
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_tree_build error");
	}
	tree = git_mysql_tree_root(mysql, repo);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_tree_build error");
	}

	git_tree_free(tree);

	git_mysql_commit(mysql);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());

}

NAN_METHOD(GitMysql::BranchLookup) {
	NanEscapableScope();

	int				  error;
	git_oid			  oid;
	git_reference *	  ref;

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("branch is required.");
	}

	// start convert_from_v8 block
	const char *from_branch;
	String::Utf8Value branch_buf(args[0]->ToString());
	from_branch = (const char *)strdup(*branch_buf);
	// end convert_from_v8 block

	error = git_branch_lookup(&ref, repo, from_branch, GIT_BRANCH_LOCAL);
	if (error < 0){
		return NanThrowError("git_branch_lookup error");
	}

	git_reference_free(ref);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::CreateRef) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("name is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("target is required.");
	}

	// start convert_from_v8 block
	const char *from_name;
	String::Utf8Value name_buf(args[0]->ToString());
	from_name = (const char *)strdup(*name_buf);

	const char *from_target;
	String::Utf8Value target_buf(args[1]->ToString());
	from_target = (const char *)strdup(*target_buf);
	// end convert_from_v8 block

	int				  error;
	//git_oid			  oid;
	git_reference *ref = NULL;

	//git_oid_fromstr(&oid, from_target);

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_reference_symbolic_create(&ref, repo, from_name, from_target, 0, NULL, NULL);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_reference_symbolic_create error");
	}

	git_mysql_commit(mysql);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::Commit) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("ref is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("message is required.");
	}

	// start convert_from_v8 block
	const char *from_ref;
	String::Utf8Value ref_buf(args[0]->ToString());
	from_ref = (const char *)strdup(*ref_buf);

	const char *from_msg;
	String::Utf8Value msg_buf(args[1]->ToString());
	from_msg = (const char *)strdup(*msg_buf);
	// end convert_from_v8 block

	int				  error;
	git_oid oid;
	git_signature *me;
	git_oid commit;
	git_tree *tree;

	//error = git_revparse_single((git_object**)&tree, repo, "HEAD~^{tree}");
	//if (error < 0){
	//	git_mysql_rollback(mysql);
	//	return NanThrowError("git_revparse_single error");
	//}
	git_oid_fromstr(&oid, "b72e349a56fca69c0f1e150f49fc9afe50d7316b");


	error = git_tree_lookup(&tree, repo, &oid);
	if (error < 0){
		return NanThrowError("git_tree_lookup error");
	}

	error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
	if (error < 0){
		return NanThrowError("git_signature_now error");
	}

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_commit_create(&commit, repo, from_ref, me, me, "UTF-8", from_msg, tree, 0, NULL);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_commit_create error");
	}

	git_mysql_commit(mysql);

	char sha1[GIT_OID_HEXSZ + 1] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, &commit);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);
}


NAN_METHOD(GitMysql::CreateBranch) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("name is required.");
	}

	// start convert_from_v8 block
	const char *from_name;
	String::Utf8Value name_buf(args[0]->ToString());
	from_name = (const char *)strdup(*name_buf);
	// end convert_from_v8 block

	int				  error;
	git_oid				oid;
	git_reference	   *ref;
	git_commit	    *commit;

	git_oid_fromstr(&oid, "5e5af8327eb6ea85d85ea94fd2f64c7317ff3867");

	error = git_commit_lookup(&commit, repo, &oid);
	if (error < 0){
		return NanThrowError("git_commit_lookup error");
	}

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_branch_create(&ref, repo, from_name,commit,0,NULL,NULL);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_branch_create error");
	}

	git_mysql_commit(mysql);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::AdHoc) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("cmd is required.");
	}

	// start convert_from_v8 block
	const char *from_cmd;
	String::Utf8Value cmd_buf(args[0]->ToString());
	from_cmd = (const char *)strdup(*cmd_buf);
	// end convert_from_v8 block

	int error = git_mysql_adhoc(mysql,from_cmd);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::CreateRepo) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsNumber()) {
		return NanThrowError("owner is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("name is required.");
	}

	if (args.Length() == 2 || !args[2]->IsString()) {
		return NanThrowError("description is required.");
	}

	// start convert_from_v8 block
	long long int from_owner;
	from_owner = (long long int)args[0]->ToNumber()->Value();

	const char *from_name;
	String::Utf8Value name_buf(args[1]->ToString());
	from_name = (const char *)strdup(*name_buf);

	const char *from_desc;
	String::Utf8Value desc_buf(args[2]->ToString());
	from_desc = (const char *)strdup(*desc_buf);
	// end convert_from_v8 block

	// Transaction Start
	git_mysql_transaction(mysql);

	char *reponum = git_mysql_repo_create(mysql, from_owner, from_name, from_desc);

	if (reponum){

		mutex.lock();
		mysql->repo = std::stoi(reponum);
		mutex.unlock();

		int error;
		git_oid oid;
		git_reference *ref;
		git_tree *tree;
		git_signature *me;
		git_oid commit;
		error = git_reference_symbolic_create(&ref, repo, "HEAD", "refs/heads/master", 0, NULL, NULL);
		git_reference_free(ref);

		error = git_blob_create_frombuffer(&oid, repo, from_name, strlen(from_name));
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_blob_create_frombuffer error");
		}

		error = git_mysql_index_add(mysql, &oid, "/README.md");
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_mysql_index_write error");
		}

		error = git_mysql_tree_init(mysql);
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_mysql_tree_init error");
		}

		error = git_mysql_tree_build(mysql, repo, "BLOB");
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_mysql_tree_build error");
		}
		error = git_mysql_tree_build(mysql, repo, "TREE");
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_mysql_tree_build error");
		}
		tree = git_mysql_tree_root(mysql, repo);
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_mysql_tree_build error");
		}

		error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
		if (error < 0){
			return NanThrowError("git_signature_now error");
		}

		error = git_commit_create(&commit, repo, "refs/heads/master", me, me, "UTF-8", "Initial Commit", tree, 0, NULL);
		if (error < 0){
			git_mysql_rollback(mysql);
			return NanThrowError("git_commit_create error");
		}

		git_mysql_commit(mysql);

		Handle<v8::Value> to;
		to = NanNew<Number>(mysql->repo);
		NanReturnValue(to);
	}
	else{
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_repo_create error");
	}
}

NAN_METHOD(GitMysql::GetRepo) {
	NanEscapableScope();

	Handle<v8::Value> to;
	to = NanNew<Number>(mysql->repo);
	NanReturnValue(to);
}

NAN_METHOD(GitMysql::SetRepo) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsNumber()) {
		return NanThrowError("repo is required.");
	}

	mutex.lock();
	mysql->repo = (long long int)args[0]->ToNumber()->Value();
	mutex.unlock();
}

NAN_METHOD(GitMysql::TreeWalk) {
	NanEscapableScope();

	git_mysql_tree_walk(mysql, repo);

}


NAN_METHOD(GitMysql::Config) {
	NanEscapableScope();

	int error;
	git_config *cfg;
	git_config_backend *cfg_backend;

	error = git_mysql_config_init(&cfg_backend, mysql);
	if (error < 0){
		return NanThrowError("git_mysql_config_init error");
	}

	error = git_config_new(&cfg);
	if (error < 0){
		return NanThrowError("git_config_new error");
	}

	error = git_config_add_backend(cfg, cfg_backend, GIT_CONFIG_LEVEL_LOCAL, 0);
	if (error < 0){
		return NanThrowError("git_config_add_backend error");
	}

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_config_set_string(cfg, "user", "jerry");
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_config_set_string error");
	}

	git_mysql_commit(mysql);

	git_config_free(cfg);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::RevParse) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("spec is required.");
	}

	// start convert_from_v8 block
	const char *from_spec;
	String::Utf8Value spec_buf(args[0]->ToString());
	from_spec = (const char *)strdup(*spec_buf);
	// end convert_from_v8 block

	int error;

	git_object *tree;
	const git_oid *oid;

	error = git_revparse_single(&tree, repo, from_spec);
	if (!error)
		oid = git_tree_id((git_tree *)tree);

	char sha1[GIT_OID_HEXSZ + 1] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, oid);

	git_object_free(tree);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);

}

NAN_METHOD(GitMysql::DeleteRepo) {
	NanEscapableScope();

	int error;

	// Transaction Start
	git_mysql_transaction(mysql);

	error = git_mysql_repo_del(mysql);
	if (error < 0){
		git_mysql_rollback(mysql);
		return NanThrowError("git_mysql_repo_del error");
	}

	git_mysql_commit(mysql);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

Persistent<Function> GitMysql::constructor_template;
git_mysql * GitMysql::mysql;
git_repository * GitMysql::repo;
std::mutex GitMysql::mutex;
