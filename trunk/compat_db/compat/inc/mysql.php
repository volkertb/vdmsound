<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  include_once('inc/mysql.conf');

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Validation
  ////////////////////////////////////////////////////////////////////////////

  function MysqlIsValidInteger($integer) {
    return eregi('^[\+\-]*[0-9]+$', $integer);     // accept integers only
  }

  function MysqlIsValidSQLName($name) {
    return eregi('^[\$0-9A-Z_]+$', $name);         // accept ANSI SQL only
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - General
  ////////////////////////////////////////////////////////////////////////////

  //
  // Open connection to database
  //
  function MysqlConnect() {
    global $mysql_linkid, $mysql_conf_server, $mysql_conf_dbname, $mysql_conf_user, $mysql_conf_passwd;
    ErrSetLastError();

    if (!($mysql_linkid = mysql_connect($mysql_conf_server, $mysql_conf_user, $mysql_conf_passwd))) {
      ErrSetLastError(E_MYSQL_CANT_CONNECT);
      return false;
    }

    if (!mysql_select_db($mysql_conf_dbname, $mysql_linkid)) {
      ErrSetLastError(E_MYSQL_CANT_SEL_DB);
      return false;
    }

    return true;
  }

  //
  // Close connection to mysql server
  //
  function MysqlClose() {
    global $mysql_linkid;
    ErrSetLastError();

    if (!$mysql_linkid) {
      ErrSetLastError(E_MYSQL_NO_CONNECTION);
      return false;
    }

    if (mysql_close($mysql_linkid)) {
      return true;
    } else {
      ErrSetLastError(E_MYSQL_CANT_DISCONNECT);
      return false;
    }
  }

  //
  // Perform an SQL query on the mysql database
  //
  function MysqlQuery($query) {
    global $mysql_linkid;
    ErrSetLastError();

    if (!($result = mysql_query($query, $mysql_linkid))) {
      ErrSetLastError(E_MYSQL_CANT_QUERY_DB);
      return false;
    } else {
      return $result;
    }
  }
?>
