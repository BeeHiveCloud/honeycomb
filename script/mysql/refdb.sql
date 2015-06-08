DROP TABLE IF EXISTS `GIT_REFDB`;

CREATE TABLE `GIT_REFDB`(
  `repo` BIGINT UNSIGNED NOT NULL,
  `name` VARCHAR(255) NOT NULL,
  `type` tinyint(1) unsigned NOT NULL,
  `target`  VARCHAR(255) NOT NULL,
  PRIMARY KEY (`repo`,`name`),
  KEY `target` (`repo`,`target`)
)
ENGINE = "InnoDB"
DEFAULT CHARSET = utf8
COLLATE = utf8_bin;
