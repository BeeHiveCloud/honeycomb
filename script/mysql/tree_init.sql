DELIMITER $$

DROP PROCEDURE IF EXISTS GIT_TREE_INIT;

CREATE PROCEDURE GIT_TREE_INIT (IN p_repo BIGINT UNSIGNED)
BEGIN

DECLARE v_count INT UNSIGNED DEFAULT 1;

 DELETE FROM GIT_TREE WHERE repo = p_repo;

 INSERT INTO GIT_TREE(`repo`, `dir`, `type`, `entry`, `oid`)
 SELECT `repo`, `dir`, 'BLOB',`file`, `oid`
 from GIT_INDEX_V where repo = p_repo;

 WHILE v_count > 0 DO
	select count(1) into v_count
	  from (select distinct SUBSTRING_INDEX(`c`, '/', -1) child,
	                        LEFT(`c`, LENGTH(`c`)-LOCATE('/', REVERSE(`c`))+1) parent,
	                        repo
	          from (select distinct p.dir p, substring(c.dir,1,length(c.dir)-1)  c, c.repo
	                  from GIT_TREE p right join GIT_TREE c
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
	                  from GIT_TREE p right join GIT_TREE c
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
