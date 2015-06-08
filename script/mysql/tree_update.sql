DELIMITER $$

DROP PROCEDURE IF EXISTS GIT_TREE_UPDATE;

CREATE PROCEDURE GIT_TREE_UPDATE (IN p_repo BIGINT UNSIGNED, IN p_dir VARCHAR(255), IN p_oid BINARY(20))
BEGIN

DECLARE v_dir VARCHAR(255);
DECLARE v_entry VARCHAR(255);

UPDATE GIT_TREE
   SET tree = p_oid
 WHERE repo = p_repo
   AND dir = p_dir;

SET v_dir = substring(p_dir,1,length(p_dir)-1);
SET v_entry = SUBSTRING_INDEX(v_dir, '/', -1);
SET v_dir = LEFT(v_dir, LENGTH(v_dir)-LOCATE('/', REVERSE(v_dir))+1);

UPDATE GIT_TREE
   SET oid = p_oid
 WHERE repo = p_repo
   AND dir = v_dir
   AND entry = v_entry;

END$$

DELIMITER ;
