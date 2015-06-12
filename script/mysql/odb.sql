DROP TABLE IF EXISTS `GIT_ODB`;

CREATE TABLE `GIT_ODB` (
 `repo` BIGINT UNSIGNED NOT NULL,
 `oid`  BINARY(20) NOT NULL,
 `type` TINYINT(1) UNSIGNED NOT NULL,
 `size` INT UNSIGNED NOT NULL,
 `data` LONGBLOB NOT NULL,
 PRIMARY KEY (`repo`,`oid`),
 KEY `type` (`repo`,`type`),
 KEY `size` (`repo`,`size`)
)
ENGINE = "InnoDB"
DEFAULT CHARSET=utf8
COLLATE=utf8_bin;
