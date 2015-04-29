DELIMITER $$

DROP PROCEDURE IF EXISTS git_repo_del;

CREATE PROCEDURE git_repo_del (IN p_repo BIGINT UNSIGNED)
BEGIN

DELETE from git_odb where repo = p_repo;

DELETE from git_index where repo = p_repo;

DELETE from git_tree where repo = p_repo;

DELETE from git_refdb where repo = p_repo;

DELETE from git_config where repo = p_repo;

DELETE from git_repo where id = p_repo;

END$$

DELIMITER ;
