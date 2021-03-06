<?php

$relPath='../../../pinc/';
include_once($relPath.'base.inc');
include_once($relPath.'dpsql.inc');

echo "Creating 'authors' table...\n";

dpsql_query("
    CREATE TABLE authors (
      author_id     MEDIUMINT(8) UNSIGNED NOT NULL AUTO_INCREMENT,
      other_names   VARCHAR(40)           NOT NULL DEFAULT '',
      last_name     VARCHAR(25)           NOT NULL DEFAULT '',
      byear         MEDIUMINT(9)          NOT NULL DEFAULT '0',
      bmonth        TINYINT(4)            NOT NULL DEFAULT '0',
      bday          TINYINT(4)            NOT NULL DEFAULT '0',
      bcomments     VARCHAR(20)           NOT NULL DEFAULT '',
      dyear         MEDIUMINT(9)          NOT NULL DEFAULT '0',
      dmonth        TINYINT(4)            NOT NULL DEFAULT '0',
      dday          TINYINT(4)            NOT NULL DEFAULT '0',
      dcomments     VARCHAR(20)           NOT NULL DEFAULT '',
      enabled       TINYTEXT              NOT NULL,
      last_modified TIMESTAMP(14)         NOT NULL,

      PRIMARY KEY author_id (author_id)
    ) TYPE=MyISAM
") or die("Aborting\n");

echo "Creating 'biographies' table...\n";

dpsql_query("
    CREATE TABLE biographies (
      bio_id        INT(11)       NOT NULL AUTO_INCREMENT,
      author_id     INT(11)       NOT NULL DEFAULT '0',
      bio           TEXT          NOT NULL,
      last_modified TIMESTAMP(14) NOT NULL,

      PRIMARY KEY bio_id (bio_id)
    ) TYPE=MyISAM COMMENT='Contains biographies (see authors)'
") or die("Aborting\n");

echo "\nDone!\n";

// vim: sw=4 ts=4 expandtab
?>
