<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/error.php');
  require_once('inc/mysql.php');

  ////////////////////////////////////////////////////////////////////////////
  // One-time initial setup
  ////////////////////////////////////////////////////////////////////////////

  MysqlConnect();

  ////////////////////////////////////////////////////////////////////////////
  // Methods - General
  ////////////////////////////////////////////////////////////////////////////

  function NewsGetItems($sortkey = NULL,
                        $sortascend = true,
                        $start = 0,
                        $limit = -1 )
  {
    ErrSetLastError();

    if ((!is_null($sortkey)  && !MysqlIsValidSQLName($sortkey))  ||
        (!MysqlIsValidInteger($start)) ||
        (!MysqlIsValidInteger($limit)))  // Check against SQL code injection
    {
      ErrSetLastError(E_UNEXPECTED);
      return false;
    }

    $querystr = 'SELECT id,time_created AS created,DATE_FORMAT(time_updated,"%Y-%m-%d %H:%i:%s") AS updated,title,comment FROM News WHERE 1';

    if (!is_null($sortkey)) {
      $querystr .= ' ORDER BY ' . $sortkey;

      if ($ascending) {
        $querystr .= ' ASC';
      } else {
        $querystr .= ' DESC';
      }
    }

    if ($limit > 0) {
      $querystr .= ' LIMIT ' . $start . ',' . $limit;
    }

    // Get news contents
    return MysqlQueryAssoc($querystr);
  }
?>
