<?php

// One-time script to create 'special_days' table

$relPath='../../../pinc/';
include_once($relPath.'base.inc');
include_once($relPath.'dpsql.inc');

// -----------------------------------------------
// Create 'special_days' table.

echo "Creating 'special_days' table...\n";
dpsql_query("
CREATE TABLE special_days (
  spec_code varchar(20) NOT NULL default '',
  display_name varchar(80) NOT NULL default '',
  enable tinyint(1) NOT NULL default '1',
  comment varchar(255) default NULL,
  color varchar(8) NOT NULL default '',
  open_day tinyint(2) default NULL,
  open_month tinyint(2) default NULL,
  close_day tinyint(2) default NULL,
  close_month tinyint(2) default NULL,
  date_changes varchar(100) default NULL,
  info_url varchar(255) default NULL,
  image_url varchar(255) default NULL,
  UNIQUE KEY spec_code (spec_code)
) TYPE=MyISAM COMMENT='definitions of SPECIAL days';
    
    
") or die("Aborting.");


echo "\nDone!\n";

// vim: sw=4 ts=4 expandtab
?>
