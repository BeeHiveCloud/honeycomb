DROP TABLE IF EXISTS `GIT_CONFIG`;

CREATE TABLE GIT_CONFIG(
  `repo` BIGINT UNSIGNED NOT NULL,
  `key`  VARCHAR(255) NOT NULL,
  `value` VARCHAR(255) NOT NULL,
  PRIMARY KEY (`repo`,`key`)
)
ENGINE = "InnoDB"
DEFAULT CHARSET = utf8
COLLATE = utf8_bin;
