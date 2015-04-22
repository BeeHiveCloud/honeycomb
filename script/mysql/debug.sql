create table debug_log(
`id` SERIAL,
`message` varchar(255),
`ts` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
primary key (`id`)
)
ENGINE = "InnoDB"
DEFAULT CHARSET = utf8
COLLATE = utf8_bin;

DELIMITER $$

DROP PROCEDURE IF EXISTS log_msg;

CREATE PROCEDURE log_msg (IN p_msg VARCHAR(255))
BEGIN

insert into debug_log(message) values (p_msg);

END$$

DELIMITER ;