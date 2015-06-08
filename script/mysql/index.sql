DROP TABLE IF EXISTS `GIT_INDEX`;

CREATE TABLE GIT_INDEX(
  `repo`  BIGINT UNSIGNED NOT NULL,
  `oid`   BINARY(20) NOT NULL,
  `path`  VARCHAR(255) NOT NULL,
  PRIMARY KEY (`repo`,`path`),
  KEY `oid` (`repo`,`oid`)
)
ENGINE = "InnoDB"
DEFAULT CHARSET = utf8
COLLATE = utf8_bin;

DROP VIEW IF EXISTS `GIT_INDEX_V`;

CREATE VIEW GIT_INDEX_V AS
SELECT `repo`,`oid`,
	  (LENGTH(`path`)-LENGTH(REPLACE(`path`,'/',''))) as `level`,
	  LEFT(`path`, LENGTH(`path`)-LOCATE('/', REVERSE(`path`))+1) as `dir`,
	  SUBSTRING_INDEX(`path`, '/', -1) as `file`
FROM GIT_INDEX;
