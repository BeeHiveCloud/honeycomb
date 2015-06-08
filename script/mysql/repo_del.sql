DELIMITER $$

DROP PROCEDURE IF EXISTS GIT_REPO_DEL;

CREATE PROCEDURE GIT_REPO_DEL (IN p_repo BIGINT UNSIGNED)
BEGIN

DELETE from GIT_ODB where repo = p_repo;

DELETE from GIT_INDEX where repo = p_repo;

DELETE from GIT_TREE where repo = p_repo;

DELETE from GIT_REFDB where repo = p_repo;

DELETE from GIT_CONFIG where repo = p_repo;

DELETE from GIT_REPO where id = p_repo;

END$$

DELIMITER ;
