DROP TABLE IF EXISTS `GIT_TREE`;

create table `GIT_TREE`(
  `repo`  BIGINT UNSIGNED NOT NULL,
  `dir`   VARCHAR(255) NOT NULL,
  `tree`  BINARY(20),
  `type`  CHAR(4),
  `entry` VARCHAR(255) NOT NULL,
  `oid`   BINARY(20),
  PRIMARY KEY (`repo`,`dir`,`entry`),
  KEY (`repo`,`tree`,`oid`)
  )
ENGINE = "InnoDB"
DEFAULT CHARSET=utf8
COLLATE=utf8_bin;


-- select repo, parent, child
--  from (select distinct SUBSTRING_INDEX(`c`, '/', -1) child,
--                        LEFT(`c`, LENGTH(`c`)-LOCATE('/', REVERSE(`c`))+1) parent,
--                        repo
--          from (select distinct p.dir p, substring(c.dir,1,length(c.dir)-1)  c, c.repo
--                  from git_tree p right join git_tree c
--                    on p.repo = c.repo
--                   and concat(p.dir,p.entry,'/') = c.dir
--				   )d
--         where p is null
--	    ) t
-- where parent is not null and trim(parent) <> ''
-- and repo=1;



-- select dir, SUBSTRING_INDEX(`c`, '/', -1) entry, tree
--   from(select distinct p.dir, substring(c.dir,1,length(c.dir)-1) c,c.tree,c.repo
--	       from git_tree p right join git_tree c
--	         on p.repo = c.repo
--	        and concat(p.dir,p.entry,'/') = c.dir) d
--  where dir is not null
--    and repo = 1;
