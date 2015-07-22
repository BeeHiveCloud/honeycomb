#include "git_mysql.h"
#include "mysql.h"

void GitMysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<Object> object = NanNew<Object>();

	NODE_SET_METHOD(object, "LastError", LastError);

  NODE_SET_METHOD(object, "Init", Init);

  NODE_SET_METHOD(object, "Close", Close);

	NODE_SET_METHOD(object, "Add", CreateBlob);

	NODE_SET_METHOD(object, "WriteTree", WriteTree);

	NODE_SET_METHOD(object, "BranchLookup", BranchLookup);

	NODE_SET_METHOD(object, "CreateRef", CreateRef);

	NODE_SET_METHOD(object, "RevParse", RevParse);

	NODE_SET_METHOD(object, "Commit", Commit);

	NODE_SET_METHOD(object, "CreateBranch", CreateBranch);

	NODE_SET_METHOD(object, "CreateRepo", CreateRepo);

	NODE_SET_METHOD(object, "DeleteRepo", DeleteRepo);

	NODE_SET_METHOD(object, "CreateTag", CreateTag);

	NODE_SET_METHOD(object, "Diff", Diff);

	target->Set(NanNew<String>("GIT"), object);
}

NAN_METHOD(GitMysql::Init){
  NanEscapableScope();

  git_mysql_init(&mysql,Mysql::db);

  if (git_libgit2_init() < 0)
	  printf("git_libgit2_init error");

  git_odb_backend   *odb_backend;
  if (git_mysql_odb_init(&odb_backend, mysql) < 0)
	  printf("git_mysql_odb_init error");

  if (git_odb_new(&odb_backend->odb) < 0)
	  printf("git_odb_new error");

  if (git_odb_add_backend(odb_backend->odb, odb_backend, 1) < 0)
	  printf("git_odb_add_backend error");

  if (git_repository_wrap_odb(&repo, odb_backend->odb) < 0)
	  printf("git_repository_wrap_odb error");


  git_refdb *refdb;
  if (git_refdb_new(&refdb,repo) < 0)
	  printf("git_refdb_new error");


  git_refdb_backend   *refdb_backend;
  if (git_mysql_refdb_init(&refdb_backend, mysql) < 0)
	  printf("git_mysql_refdb_init error");

  if (git_refdb_set_backend(refdb, refdb_backend) < 0)
	  printf("git_mysql_refdb_init error");


  git_repository_set_refdb(repo, refdb);

  git_config *cfg;
  if (git_config_open_default(&cfg) < 0)
	  printf("git_config_open_default error");

  git_repository_set_config(repo,cfg);

  if (git_repository_set_workdir(repo, "/", 0) < 0)
	  printf("git_repository_set_workdir error");

  if (git_repository_set_path(repo, "/") < 0)
	  printf("git_repository_set_path error");

}


NAN_METHOD(GitMysql::Close) {
  NanEscapableScope();
  git_mysql_free(mysql);
}

NAN_METHOD(GitMysql::LastError) {
    NanEscapableScope();

    const git_error *e = giterr_last();

    Handle<v8::Value> to;
    to = NanNew<String>(e->message);

    NanReturnValue(to);
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
	mysql_trx_start(mysql->db);

	error = git_blob_create_frombuffer(&oid, repo, from_buf, strlen(from_buf));
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_blob_create_frombuffer error");
	}

	error = git_mysql_index_write(mysql, &oid, from_path);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_index_write error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_path);
    free((char *)from_buf);

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
	mysql_trx_start(mysql->db);

	error = git_mysql_tree_init(mysql);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_init error");
	}

	error = git_mysql_tree_build(mysql, repo, "BLOB");
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}
	error = git_mysql_tree_build(mysql, repo, "TREE");
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}
	tree = git_mysql_tree_root(mysql, repo);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}

	git_tree_free(tree);

	mysql_trx_commit(mysql->db);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());

}

NAN_METHOD(GitMysql::BranchLookup) {
	NanEscapableScope();

	int				  error;
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

  free((char *)from_branch);

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
	mysql_trx_start(mysql->db);

	error = git_reference_symbolic_create(&ref, repo, from_name, from_target, 0, NULL);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_reference_symbolic_create error");
	}

	mysql_trx_commit(mysql->db);

  free((char *)from_name);
  free((char *)from_target);

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

	int	error;
	git_signature *me;
	git_oid commit;
	git_tree *tree;
	git_object *parent;

	error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
	if (error < 0){
		return NanThrowError("git_signature_now error");
	}

	error = git_revparse_single(&parent, repo, "HEAD^{commit}");
	if (error < 0){
		return NanThrowError("git_revparse_single error");
	}

	// Transaction Start
	mysql_trx_start(mysql->db);

	error = git_mysql_tree_init(mysql);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_init error");
	}

	error = git_mysql_tree_build(mysql, repo, "BLOB");
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}
	error = git_mysql_tree_build(mysql, repo, "TREE");
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}
	tree = git_mysql_tree_root(mysql, repo);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build error");
	}

	const git_commit *parents[] = { (git_commit *)parent };
	error = git_commit_create(&commit, repo, from_ref, me, me, "UTF-8", from_msg, tree, 1, parents);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_commit_create error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_ref);
    free((char *)from_msg);
    git_signature_free(me);

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
	mysql_trx_start(mysql->db);

	error = git_branch_create(&ref, repo, from_name,commit,0);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_branch_create error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_name);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::CreateRepo) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("name is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("description is required.");
	}

	// start convert_from_v8 block
	const char *from_name;
	String::Utf8Value name_buf(args[0]->ToString());
	from_name = (const char *)strdup(*name_buf);

	const char *from_desc;
	String::Utf8Value desc_buf(args[1]->ToString());
	from_desc = (const char *)strdup(*desc_buf);
	// end convert_from_v8 block

	// Transaction Start
	mysql_trx_start(mysql->db);

	my_ulonglong rid = git_mysql_repo_create(mysql, from_name, from_desc);

	if (rid > 0){
        char buf[100];
        memset(buf,0,sizeof(buf));
        sprintf(buf,"%llu",rid);
        mysql_set_variable(mysql->db, "repo", buf);

		int error;
		git_oid oid;
		git_reference *ref;
		git_tree *tree;
		git_signature *me;
		//git_oid commit;
		error = git_reference_symbolic_create(&ref, repo, "HEAD", "refs/heads/master", 0, NULL);
    git_reference_free(ref);

		error = git_blob_create_frombuffer(&oid, repo, from_name, strlen(from_name));
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_blob_create_frombuffer error");
		}

		error = git_mysql_index_write(mysql, &oid, "/README.md");
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_index_write error");
		}

		error = git_mysql_tree_init(mysql);
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_init error");
		}

		error = git_mysql_tree_build(mysql, repo, "BLOB");
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_build error");
		}
		error = git_mysql_tree_build(mysql, repo, "TREE");
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_build error");
		}
		tree = git_mysql_tree_root(mysql, repo);
		if (!tree){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_build error");
		}

		error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
		if (error < 0){
			return NanThrowError("git_signature_now error");
		}
		//printf("489\n");
		error = git_commit_create(NULL, repo, "refs/heads/master", me, me, NULL, "Initial Commit", tree, 0, NULL);
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_commit_create error");
		}

		mysql_trx_commit(mysql->db);

        free((char *)from_name);
        free((char *)from_desc);
        git_signature_free(me);
		git_tree_free(tree);

		Handle<v8::Value> to;
		to = NanNew<Number>(rid);
		NanReturnValue(to);
	}
	else{
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_repo_create error");
	}
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
	const git_oid *oid = NULL;

	error = git_revparse_single(&tree, repo, from_spec);
	if (!error)
		oid = git_tree_id((git_tree *)tree);

	char sha1[GIT_OID_HEXSZ + 1] = { 0 };
	git_oid_tostr(sha1, GIT_OID_HEXSZ + 1, oid);

	git_object_free(tree);
    free((char *)from_spec);

	Handle<v8::Value> to;
	to = NanNew<String>(sha1);

	NanReturnValue(to);

}

NAN_METHOD(GitMysql::DeleteRepo) {
	NanEscapableScope();

	int error;

	// Transaction Start
	mysql_trx_start(mysql->db);

	error = git_mysql_repo_del(mysql);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_repo_del error");
	}

	mysql_trx_commit(mysql->db);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::CreateTag) {
	NanEscapableScope();

	if (args.Length() == 0 || !args[0]->IsString()) {
		return NanThrowError("tag is required.");
	}

	if (args.Length() == 1 || !args[1]->IsString()) {
		return NanThrowError("message is required.");
	}

	// start convert_from_v8 block
	const char *from_tag;
	String::Utf8Value tag_buf(args[0]->ToString());
	from_tag = (const char *)strdup(*tag_buf);

	const char *from_msg;
	String::Utf8Value msg_buf(args[1]->ToString());
	from_msg = (const char *)strdup(*msg_buf);
	// end convert_from_v8 block

	int error;
	git_oid oid;
	git_object *target = NULL;
	git_signature *tagger = NULL;

	error = git_revparse_single(&target, repo, "HEAD^{commit}");
	if (error < 0){
		return NanThrowError("git_revparse_single error");
	}

	error = git_signature_now(&tagger,"Jerry Jin", "jerry.yang.jin@gmail.com");

	// Transaction Start
	mysql_trx_start(mysql->db);

	error = git_tag_create(&oid, repo, from_tag, target, tagger, from_msg, 0);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_tag_create error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_tag);
    free((char *)from_msg);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

NAN_METHOD(GitMysql::Diff) {
	NanEscapableScope();

	int error;
	git_object *commit;
	git_commit *parent = NULL;
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
		return NanThrowError("git_diff_tree_to_tree error");
	}

	git_mysql_tree_diff(diff);
	if (error < 0){
		return NanThrowError("git_mysql_tree_diff error");
	}

	git_object_free(commit);
	git_commit_free(parent);
	git_tree_free(commit_tree);
	git_tree_free(parent_tree);
	git_diff_free(diff);

	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

Persistent<Function> GitMysql::constructor_template;
git_mysql * GitMysql::mysql;
git_repository * GitMysql::repo;
