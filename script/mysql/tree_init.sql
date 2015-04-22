DELIMITER $$

DROP PROCEDURE IF EXISTS git_tree_init;

CREATE PROCEDURE git_tree_init (IN p_repo BIGINT UNSIGNED)
BEGIN

DECLARE v_count INT UNSIGNED DEFAULT 1;

 DELETE FROM GIT_TREE WHERE repo = p_repo;

 INSERT INTO GIT_TREE(`repo`, `dir`, `type`, `entry`, `oid`)
 SELECT `repo`, `dir`, 'BLOB',`file`, `oid`
 from git_index_v where repo = p_repo;

 WHILE v_count > 0 DO
	select count(1) into v_count
	  from (select distinct SUBSTRING_INDEX(`c`, '/', -1) child,
	                        LEFT(`c`, LENGTH(`c`)-LOCATE('/', REVERSE(`c`))+1) parent,
	                        repo
	          from (select distinct p.dir p, substring(c.dir,1,length(c.dir)-1)  c, c.repo
	                  from git_tree p right join git_tree c
	                    on p.repo = c.repo
	                   and concat(p.dir,p.entry,'/') = c.dir
					   )d
	         where p is null
		    ) t
	 where parent is not null and trim(parent) <> ''
	 and repo = p_repo;

 INSERT INTO GIT_TREE(`repo`, `dir`, `type`,`entry`)
	select repo, parent, 'TREE',child
	  from (select distinct SUBSTRING_INDEX(`c`, '/', -1) child,
	                        LEFT(`c`, LENGTH(`c`)-LOCATE('/', REVERSE(`c`))+1) parent,
	                        repo
	          from (select distinct p.dir p, substring(c.dir,1,length(c.dir)-1)  c, c.repo
	                  from git_tree p right join git_tree c
	                    on p.repo = c.repo
	                   and concat(p.dir,p.entry,'/') = c.dir
					   )d
	         where p is null
		    ) t
	 where parent is not null and trim(parent) <> ''
	 and repo = p_repo;

 END WHILE;

END$$

DELIMITER ;
