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
  define('APPS_GET_APPVER',        0x00000002);
  define('APPS_GET_DISTRIB',       0x00000004);

  define('APPS_GET_OSVER',         0x00000001);
  define('APPS_GET_EMUVER',        0x00000002);

  define('APPS_GET_CMIDI',         0x00000001);
  define('APPS_GET_CSB',           0x00000002);
  define('APPS_GET_CADLIB',        0x00000004);
  define('APPS_GET_CJOYSTICK',     0x00000008);

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

  function AppsGetGlobalStats() {
    $retval = Array();

    // Count all the applications
    if ($result = MysqlQuery('SELECT COUNT(DISTINCT(Titles_id)) FROM Applications'))
      $retval['apps'] = mysql_result($result, 0, 0);

    // Count all the reports
    if ($result = MysqlQuery('SELECT COUNT(*) FROM Reports'))
      $retval['reports'] = mysql_result($result, 0, 0);

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
    $retval  = Array();

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
      array_push($columns, 'Reports.comment as comment_text');
    }

    if ($getuserinfo & APPS_GET_USER) {
      if ($getuserinfo & APPS_GET_AS_ID)
        array_push($columns, 'Users.id as user_id');
      if ($getuserinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'Users.name as user_text');

      array_push($tables, 'Users ON (Reports.Users_id=Users.id)');
    }

    if ($gettitleinfo & APPS_GET_TITLE) {
      if ($gettitleinfo & APPS_GET_AS_ID)
        array_push($columns, 'Titles.id as title_id');
      if ($gettitleinfo & APPS_GET_AS_TEXT)
        array_push($columns, 'Titles.name as title_text');

      $uses_applications = true;
      array_push($tables, 'Titles ON (Applications.Titles_id=Titles.id)');
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
    if ($result = MysqlQuery($querystr)) {
      while ($reportinfo = mysql_fetch_assoc($result)) {
        array_push($retval, $reportinfo);
      }
    }

    return $retval;
  }
?>
