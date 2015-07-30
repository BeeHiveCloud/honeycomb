#include "git_mysql.h"
#include "mysql.h"


void GitMysql::InitializeComponent(Handle<v8::Object> target) {
    NanScope();

	Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(JSNewFunction);

	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(NanNew<String>("Git"));

	// Prototype methods
	NODE_SET_PROTOTYPE_METHOD(tpl, "lastError", LastError);
	NODE_SET_PROTOTYPE_METHOD(tpl, "add", CreateBlob);
	NODE_SET_PROTOTYPE_METHOD(tpl, "commit", Commit);
	NODE_SET_PROTOTYPE_METHOD(tpl, "createBranch", CreateBranch);
	NODE_SET_PROTOTYPE_METHOD(tpl, "createRepo", CreateRepo);
	NODE_SET_PROTOTYPE_METHOD(tpl, "deleteRepo", DeleteRepo);
	NODE_SET_PROTOTYPE_METHOD(tpl, "createTag", CreateTag);
	NODE_SET_PROTOTYPE_METHOD(tpl, "diff", Diff);
	NODE_SET_PROTOTYPE_METHOD(tpl, "blame", Blame);

	// Methods
	//NODE_SET_METHOD(tpl, "dummy", Dummy);

	Local<Function> _constructor_template = tpl->GetFunction();
	NanAssignPersistent(constructor_template, _constructor_template);
	target->Set(NanNew<String>("Git"), tpl->GetFunction());
}

GitMysql::GitMysql(){
    git_mysql_init(&mysql, MySQL::db);
    git_libgit2_init();
    git_mysql_odb_init(&odb_backend, mysql);
    git_odb_new(&odb_backend->odb);
    git_odb_add_backend(odb_backend->odb, odb_backend, 1);
    git_repository_wrap_odb(&repo, odb_backend->odb);
    git_refdb_new(&refdb,repo);
    git_mysql_refdb_init(&refdb_backend, mysql);
    git_refdb_set_backend(refdb, refdb_backend);
    git_repository_set_refdb(repo, refdb);
    git_config_open_default(&cfg);
    git_repository_set_config(repo,cfg);
    git_repository_set_workdir(repo, "/", 0);
    git_repository_set_path(repo, "/");
}

GitMysql::~GitMysql(){
    git_refdb_free(refdb);
    git_odb_free(odb_backend->odb);
    git_config_free(cfg);
    mysql_odb_free(odb_backend);
    mysql_refdb_free(refdb_backend);
	git_repository_free(repo);
	git_mysql_free(mysql);
	git_libgit2_shutdown();
}

NAN_METHOD(GitMysql::JSNewFunction){
  NanScope();



  GitMysql *obj = new GitMysql();
  obj->Wrap(args.This());

  NanReturnValue(args.This());
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
		return NanThrowError("git_mysql_tree_build BLOB error");
	}
	error = git_mysql_tree_build(mysql, repo, "TREE");
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_build TREE error");
	}
	tree = git_mysql_tree_root(mysql, repo);
	if (!tree){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_mysql_tree_root error");
	}

	const git_commit *parents[] = { (git_commit *)parent };
	error = git_commit_create(&commit, repo, from_ref, me, me, NULL, from_msg, tree, 1, parents);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_commit_create error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_ref);
    free((char *)from_msg);
    git_signature_free(me);
    git_tree_free(tree);
    git_object_free(parent);

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
	git_reference	   *ref;
	git_object	    *commit;

    error = git_revparse_single(&commit, repo, "HEAD^{commit}");
    if (error < 0){
        return NanThrowError("git_revparse_single error");
    }

	// Transaction Start
	mysql_trx_start(mysql->db);

	error = git_branch_create(&ref, repo, from_name,(git_commit *)commit,0);
	if (error < 0){
		mysql_trx_rollback(mysql->db);
		return NanThrowError("git_branch_create error");
	}

	mysql_trx_commit(mysql->db);

    free((char *)from_name);
	git_object_free(commit);
	git_reference_free(ref);

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

    
    if(git_mysql_repo_exists(mysql, from_name) ==1 )
        NanReturnValue(NanNew<String>("repo already exists"));
    
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
		git_oid commit;
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
			return NanThrowError("git_mysql_tree_build BLOB error");
		}
		error = git_mysql_tree_build(mysql, repo, "TREE");
		if (error < 0){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_build TREE error");
		}
		tree = git_mysql_tree_root(mysql, repo);
		if (!tree){
			mysql_trx_rollback(mysql->db);
			return NanThrowError("git_mysql_tree_root error");
		}
		error = git_signature_now(&me, "Jerry Jin", "jerry.yang.jin@gmail.com");
		if (error < 0){
			return NanThrowError("git_signature_now error");
		}
		error = git_commit_create(&commit, repo, "refs/heads/master", me, me, NULL, "Initial Commit", tree, 0, NULL);
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

NAN_METHOD(GitMysql::DeleteRepo) {
	NanEscapableScope();

	int error = GIT_ERROR;

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
	git_signature_free(tagger);
	git_object_free(target);

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

NAN_METHOD(GitMysql::Blame) {
	NanEscapableScope();

	int error = GIT_ERROR;



	if (!error)
		NanReturnValue(NanTrue());
	else
		NanReturnValue(NanFalse());
}

Persistent<Function> GitMysql::constructor_template;
git_mysql * GitMysql::mysql;
git_repository * GitMysql::repo;
