<?php
  ////////////////////////////////////////////////////////////////////////////
  // Constants
  ////////////////////////////////////////////////////////////////////////////

  define('APPS_GET_AS_ID',         0x10000000);
  define('APPS_GET_AS_TEXT',       0x20000000);
  define('APPS_GET_AS_ICON',       0x40000000);

  define('APPS_GET_COMMENT',       0x00000001);
  define('APPS_GET_USER',          0x00000002);

  define('APPS_GET_TITLE',         0x00000001);
  define('APPS_GET_APP',           0x00000002);
  define('APPS_GET_APPVER',        0x00000004);
  define('APPS_GET_DISTRIB',       0x00000008);

  define('APPS_GET_OSVER',         0x00000001);
  define('APPS_GET_EMUVER',        0x00000002);

  define('APPS_GET_CMIDI',         0x00000001);
  define('APPS_GET_CSB',           0x00000002);
  define('APPS_GET_CADLIB',        0x00000004);
  define('APPS_GET_CJOYSTICK',     0x00000008);

  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/auth.php');
  require_once('inc/error.php');
  require_once('inc/mysql.php');

  ////////////////////////////////////////////////////////////////////////////
  // One-time initial setup
  ////////////////////////////////////////////////////////////////////////////

  MysqlConnect();

  ////////////////////////////////////////////////////////////////////////////
  // Methods - General
  ////////////////////////////////////////////////////////////////////////////

  function AppsGetGlobalStats() {
    $retval = Array();

    // Count all the applications
    if ($result = MysqlQuery('SELECT COUNT(DISTINCT(Titles_id)) FROM Applications')) {
      $retval['apps'] = mysql_result($result, 0, 0);
    } else {
      $retval['apps'] = 0;
    }

    // Count all the reports
    if ($result = MysqlQuery('SELECT COUNT(*) FROM Reports')) {
      $retval['reports'] = mysql_result($result, 0, 0);
    } else {
      $retval['reports'] = 0;
    }

    return $retval;
  }

  function AppsGetReports($reportid = NULL,
                          $userid = NULL,
                          $appid = NULL,
                          $matchall = true,
                          $getuserinfo = 0,
                          $gettitleinfo = 0,
                          $getenvinfo = 0,
                          $getcompatinfo = 0,
                          $sortkey = NULL,
                          $sortascend = true,
                          $start = 0,
                          $limit = -1 )
  {
    ErrSetLastError();

    $columns = Array();
    $tables  = Array();
    $filters = Array();

    if ((!is_null($reportid) && !MysqlIsValidInteger($reportid)) ||
        (!is_null($userid)   && !MysqlIsValidInteger($userid))   ||
        (!is_null($appid)    && !MysqlIsValidInteger($appid))    ||
        (!is_null($sortkey)  && !MysqlIsValidSQLName($sortkey))  ||
        (!MysqlIsValidInteger($start)) ||
        (!MysqlIsValidInteger($limit)))  // Check against SQL code injection
    {
      ErrSetLastError(E_UNEXPECTED);
      return false;
    }

    array_push($columns, 'Reports.id AS report_id');
    array_push($columns, 'Reports.time_created AS created');
    array_push($columns, 'DATE_FORMAT(Reports.time_updated,"%Y-%m-%d %H:%i:%s") AS updated');

    array_push($tables, 'Reports');

    $uses_applications = false;

    if ($getuserinfo & APPS_GET_COMMENT) {
      array_push($columns, 'Reports.comment AS comment_text');
    }

    if ($getuserinfo & APPS_GET_USER) {
      if ($getuserinfo & APPS_GET_AS_ID)
        array_push($columns, 'Users.id AS user_id');
      if ($getuserinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'Users.name AS user_text');

      array_push($tables, 'Users ON (Reports.Users_id=Users.id)');
    }

    if ($gettitleinfo & APPS_GET_TITLE) {
      if ($gettitleinfo & APPS_GET_AS_ID)
        array_push($columns, 'Titles.id AS title_id');
      if ($gettitleinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'Titles.name AS title_text');

      $uses_applications = true;
      array_push($tables, 'Titles ON (Applications.Titles_id=Titles.id)');
    }

    if ($gettitleinfo & APPS_GET_APP) {
      array_push($columns, 'Applications.id AS app_id');
      $uses_applications = true;
    }

    if ($gettitleinfo & APPS_GET_APPVER) {
      array_push($columns, 'Applications.version AS appver_text');
      $uses_applications = true;
    }

    if ($gettitleinfo & APPS_GET_DISTRIB) {
      if ($gettitleinfo & APPS_GET_AS_ID)
        array_push($columns, 'DistributionTypes.id AS distrib_id');
      if ($gettitleinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'DistributionTypes.description AS distrib_text');
      if ($gettitleinfo & APPS_GET_AS_ICON)
        array_push($columns, 'DistributionTypes.icon_URL AS distrib_icon');

      $uses_applications = true;
      array_push($tables, 'DistributionTypes ON (Applications.DistributionTypes_id=DistributionTypes.id)');
    }

    if ($getenvinfo & APPS_GET_OSVER) {
      if ($getenvinfo & APPS_GET_AS_ID)
        array_push($columns, 'OSVerTypes.id AS osver_id');
      if ($getenvinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'OSVerTypes.description AS osver_text');
      if ($getenvinfo & APPS_GET_AS_ICON)
        array_push($columns, 'OSVerTypes.icon_URL AS osver_icon');

      array_push($tables, 'OSVerTypes ON (Reports.OSVerTypes_id=OSVerTypes.id)');
    }

    if ($getenvinfo & APPS_GET_EMUVER) {
      if ($getenvinfo & APPS_GET_AS_ID)
        array_push($columns, 'EMUVerTypes.id AS emuver_id');
      if ($getenvinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'EMUVerTypes.description AS emuver_text');
      if ($getenvinfo & APPS_GET_AS_ICON)
        array_push($columns, 'EMUVerTypes.icon_URL AS emuver_icon');

      array_push($tables, 'EMUVerTypes ON (Reports.EMUVerTypes_id=EMUVerTypes.id)');
    }

    if ($getcompatinfo & APPS_GET_CMIDI) {
      if ($getcompatinfo & APPS_GET_AS_ID)
        array_push($columns, 'CompatTypes_midi.id AS cmidi_id');
      if ($getcompatinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'CompatTypes_midi.description AS cmidi_text');
      if ($getcompatinfo & APPS_GET_AS_ICON)
        array_push($columns, 'CompatTypes_midi.icon_URL AS cmidi_icon');

      array_push($tables, 'CompatTypes AS CompatTypes_midi ON (Reports.CompatTypes_id_midi=CompatTypes_midi.id)');
    }

    if ($getcompatinfo & APPS_GET_CSB) {
      if ($getcompatinfo & APPS_GET_AS_ID)
        array_push($columns, 'CompatTypes_sb.id AS csb_id');
      if ($getcompatinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'CompatTypes_sb.description AS csb_text');
      if ($getcompatinfo & APPS_GET_AS_ICON)
        array_push($columns, 'CompatTypes_sb.icon_URL AS csb_icon');

      array_push($tables, 'CompatTypes AS CompatTypes_sb ON (Reports.CompatTypes_id_sb=CompatTypes_sb.id)');
    }

    if ($getcompatinfo & APPS_GET_CADLIB) {
      if ($getcompatinfo & APPS_GET_AS_ID)
        array_push($columns, 'CompatTypes_adlib.id AS cadlib_id');
      if ($getcompatinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'CompatTypes_adlib.description AS cadlib_text');
      if ($getcompatinfo & APPS_GET_AS_ICON)
        array_push($columns, 'CompatTypes_adlib.icon_URL AS cadlib_icon');

      array_push($tables, 'CompatTypes AS CompatTypes_adlib ON (Reports.CompatTypes_id_adlib=CompatTypes_adlib.id)');
    }

    if ($getcompatinfo & APPS_GET_CJOYSTICK) {
      if ($getcompatinfo & APPS_GET_AS_ID)
        array_push($columns, 'CompatTypes_joystick.id AS cjoystick_id');
      if ($getcompatinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'CompatTypes_joystick.description AS cjoystick_text');
      if ($getcompatinfo & APPS_GET_AS_ICON)
        array_push($columns, 'CompatTypes_joystick.icon_URL AS cjoystick_icon');

      array_push($tables, 'CompatTypes AS CompatTypes_joystick ON (Reports.CompatTypes_id_joystick=CompatTypes_joystick.id)');
    }

    if ($uses_applications) {
      array_push($tables, 'Applications ON (Reports.Applications_id=Applications.id)');
    }

    if (!is_null($reportid))
      array_push($filters, 'Reports.id=' . $reportid);

    if (!is_null($userid))
      array_push($filters, 'Reports.Users_id=' . $userid);

    if (!is_null($appid))
      array_push($filters, 'Reports.Applications_id=' . $appid);

    if (!is_null($titleid))
      array_push($filters, 'Applications.Titles_id=' . $titleid);

    $querystr  = 'SELECT ' . implode(',', $columns);
    $querystr .= ' FROM '  . implode(' INNER JOIN ', $tables);

    if (count($filters) > 0) {
      $querystr .= ' WHERE ';

      if ($matchall) {
        $querystr .= implode(' AND ', $filters);
      } else {
        $querystr .= implode(' OR ', $filters);
      }
    }

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

    // Get time, title, distribution, version, OS and emulator
    return MysqlQueryAssoc($querystr);
  }

  //
  // Gets application info given an ID
  //
  function AppsGetByID($appid) {
    ErrSetLastError();

    if (!is_null($appid) && !MysqlIsValidInteger($appid)) {  // Check against SQL code injection
      ErrSetLastError(E_UNEXPECTED);
      return false;
    }

    $querystr = 'SELECT Titles.name AS title_text,Titles.id AS name_id,Applications.id AS app_id,Applications.version AS appver_text,Applications.time_created AS created,DATE_FORMAT(Applications.time_updated,"%Y-%m-%d %H:%i:%s") AS updated,DistributionTypes.id AS distrib_id,DistributionTypes.description AS distrib_text,DistributionTypes.icon_URL AS distrib_icon,COUNT(Reports.id) AS reports_num FROM Titles INNER JOIN Applications ON (Titles.id=Applications.Titles_id) INNER JOIN DistributionTypes ON (DistributionTypes.id=Applications.DistributionTypes_id) LEFT JOIN Reports ON (Applications.id=Reports.Applications_id) WHERE Applications.id="' . $appid . '" GROUP BY Reports.Applications_id';

    if (!($result = MysqlQuery($querystr)))
      return false;

    return mysql_fetch_assoc($result);
  }

  //
  // Gets application info given a title
  //
  function AppsGetByName($appnames,
                         $operator = '=',
                         $matchall = true,
                         $sortkey = NULL,
                         $sortascend = true,
                         $start = 0,
                         $limit = -1)
  {
    ErrSetLastError();

    $filters = Array();

    if ((!is_null($sortkey) && !MysqlIsValidSQLName($sortkey))  ||
        (!MysqlIsValidInteger($start)) ||
        (!MysqlIsValidInteger($limit)))  // Check against SQL code injection
    {
      ErrSetLastError(E_UNEXPECTED);
      return false;
    }

    if (is_string($appnames))
      $appnames = Array($appnames);

    $querystr = 'SELECT Titles.name AS title_text,Titles.id AS name_id,Applications.id AS app_id,Applications.version AS appver_text,Applications.time_created AS created,DATE_FORMAT(Applications.time_updated,"%Y-%m-%d %H:%i:%s") AS updated,DistributionTypes.id AS distrib_id,DistributionTypes.description AS distrib_text,DistributionTypes.icon_URL AS distrib_icon,COUNT(Reports.id) AS reports_num FROM Titles INNER JOIN Applications ON (Titles.id=Applications.Titles_id) INNER JOIN DistributionTypes ON (DistributionTypes.id=Applications.DistributionTypes_id) INNER JOIN Reports ON (Applications.id=Reports.Applications_id) WHERE ';

    foreach ($appnames as $appname) {
      array_push($filters, 'Titles.name ' . $operator . ' "' . mysql_escape_string($appname) . '"');
    }

    if ($matchall) {
      $querystr .= implode(' AND ', $filters);
    } else {
      $querystr .= implode(' OR ', $filters);
    }

    $querystr .= ' GROUP BY Reports.Applications_id';

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

    // Perform the query and return
    return MysqlQueryAssoc($querystr);
  }

  //
  // AppsGetSimilarTitles
  //
  function AppsSoundex($string) {
    $parts = explode(' ', ereg_replace('[^[:alnum:]]', ' ', $string));
    $result = Array();

    foreach($parts as $part) {
      array_push($result, soundex($part));
    }

    return implode(' ', $result);
  }

  //
  // AppsGetSimilarTitles
  //
  function AppsGetSimilar($title) {
    $title = $title . ' ' . AppsSoundex($title);

    return MysqlQueryAssoc('SELECT Applications.id,Titles.name,Titles.nameSoundex FROM Titles INNER JOIN Applications ON (Titles.id=Applications.Titles_id) WHERE MATCH(Titles.name,Titles.nameSoundex) AGAINST("' . mysql_escape_string($title) . '")');
  }

  function AppsAddReport(&$reportid, $title, $distrib, $version, $os, $emu, $csb, $cadlib, $cmidi, $cjoystick, $comment, $forceadd = true) {
    ErrSetLastError();

    if (is_null($reportid)) {
      $reportid = 0;
    }

    if ((!MysqlIsValidInteger($reportid)) ||
        (!MysqlIsValidInteger($distrib)) ||
        (!MysqlIsValidInteger($os)) ||
        (!MysqlIsValidInteger($emu)) ||
        (!MysqlIsValidInteger($csb)) ||
        (!MysqlIsValidInteger($cadlib)) ||
        (!MysqlIsValidInteger($cmidi)) ||
        (!MysqlIsValidInteger($cjoystick)))  // Check against SQL code injection
    {
      ErrSetLastError(E_UNEXPECTED);
      return false;
    }

    $titles = MysqlQueryAssoc('SELECT id,name FROM Titles WHERE name="' . mysql_escape_string($title) . '"');

    if ($titles && (count($titles) == 1)) {
      $Titles_id = $titles[0]['id'];
    } else if ($forceadd) {
      if (!MysqlQuery('INSERT INTO Titles (name,nameSoundex) VALUES ("' . mysql_escape_string($title) . '","' . mysql_escape_string(AppsSoundex($title)) . '")'))
        return false;

      $Titles_id = mysql_insert_id();
    } else {
      return false;
    }

    $apps = MysqlQueryAssoc('SELECT id,Titles_id,version,DistributionTypes_id FROM Applications WHERE Titles_id=' . $Titles_id . ' AND version="' . mysql_escape_string($version) . '" AND DistributionTypes_id=' . $distrib);

    if ($apps && (count($apps) == 1)) {
      $Applications_id = $apps[0]['id'];
    } else if ($forceadd) {
      if (!MysqlQuery('INSERT INTO Applications (Titles_id,version,time_created,DistributionTypes_id) VALUES (' . $Titles_id . ',"' . mysql_escape_string($version) . '",NOW(),' . $distrib . ')'))
        return false;

      $Applications_id = mysql_insert_id();
    } else {
      return false;
    }

    if ($reportid > 0) {
      if (!MysqlQuery('UPDATE Reports SET Applications_id=' . $Applications_id . ',comment="' . mysql_escape_string($comment) . '",CompatTypes_id_midi=' . $cmidi . ',CompatTypes_id_sb=' . $csb . ',CompatTypes_id_adlib=' . $cadlib . ',CompatTypes_id_joystick=' . $cjoystick . ',OSVerTypes_id=' . $os . ',EMUVerTypes_id=' . $emu . ' WHERE id=' . $reportid . ' AND Users_id=' . AuthGetUserId()))
        return false;

      return true;
    } else {
      if (!MysqlQuery('INSERT INTO Reports (Users_id,Applications_id,time_created,comment,CompatTypes_id_midi,CompatTypes_id_sb,CompatTypes_id_adlib,CompatTypes_id_joystick,OSVerTypes_id,EMUVerTypes_id) VALUES (' . AuthGetUserId() . ',' . $Applications_id . ',NOW(),"' . mysql_escape_string($comment) . '",' . $cmidi . ',' . $csb . ',' . $cadlib . ',' . $cjoystick . ',' . $os . ',' . $emu . ')'))
        return false;

      $reportid = mysql_insert_id();

      return true;
    }
  }

  //
  // Remove a report
  //
  function AppsDeleteReport($reportid) {
    if (!MysqlQuery('DELETE FROM Reports WHERE id=' . $reportid . ' AND Users_id=' . AuthGetUserId()))
      return false;

    return true;
  }

  //
  // Gets various tables
  //
  function AppsGetDistributionTypes() {
    return MysqlQueryAssoc('SELECT * FROM DistributionTypes');
  }
  function AppsGetCompatTypes() {
    return MysqlQueryAssoc('SELECT * FROM CompatTypes');
  }
  function AppsGetEMUVerTypes() {
    return MysqlQueryAssoc('SELECT * FROM EMUVerTypes');
  }
  function AppsGetOSVerTypes() {
    return MysqlQueryAssoc('SELECT * FROM OSVerTypes');
  }
?>
