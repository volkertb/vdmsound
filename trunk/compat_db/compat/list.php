<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');
  require_once('inc/apps.php');

  SessionStart();

  $loggedin = AuthLogin();
?>

<?php HtmlBeginPage('Browse the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  if (!isset($i)) $i = 0;
  if (!isset($n)) $n = 10;

  if (isset($reportid)) {
    $myReports = AppsGetReports($reportid, NULL, NULL, true,
                                APPS_GET_USER   | APPS_GET_COMMENT | APPS_GET_AS_ID   | APPS_GET_AS_TEXT,
                                APPS_GET_TITLE  | APPS_GET_APP     | APPS_GET_APPVER  | APPS_GET_DISTRIB | APPS_GET_AS_TEXT   | APPS_GET_AS_ICON,
                                APPS_GET_OSVER  | APPS_GET_EMUVER  | APPS_GET_EMUCAPS | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                                APPS_GET_COMPAT | APPS_GET_AS_TEXT | APPS_GET_AS_ID   | APPS_GET_AS_ICON,
                                NULL, false);

    if ($myReports && (count($myReports) > 0)) {
      HtmlSendReportList($myReports, $loggedin, false);
    } else {
      echo('<h2 class="normal">No compatibility report matching the given ID exists</h2>');
    }
  } else if (isset($appid)) {
    if (!isset($sortkey)) $sortkey = 'updated';
    if (!isset($sortasc)) $sortasc = true;

    $myReports = AppsGetReports(NULL, NULL, $appid, true,
                                APPS_GET_USER   | APPS_GET_COMMENT | APPS_GET_AS_ID   | APPS_GET_AS_TEXT,
                                APPS_GET_TITLE  | APPS_GET_APP     | APPS_GET_APPVER  | APPS_GET_DISTRIB   | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                                APPS_GET_OSVER  | APPS_GET_EMUVER  | APPS_GET_EMUCAPS | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                                APPS_GET_COMPAT | APPS_GET_AS_TEXT | APPS_GET_AS_ID   | APPS_GET_AS_ICON,
                                $sortkey, $sortasc, $i, $n);

    if ($myReports && (count($myReports) > 0)) {
      HtmlSendReportList($myReports, $loggedin, false);
    } else {
      echo('<h2 class="normal">No compatibility report(s) matching the given ID exist(s)</h2>');
    }
  } else if (isset($userid)) {
    if (!isset($sortkey)) $sortkey = 'app_id';
    if (!isset($sortasc)) $sortasc = true;

    $myReports = AppsGetReports(NULL, $userid, NULL, true,
                                APPS_GET_USER   | APPS_GET_COMMENT | APPS_GET_AS_ID   | APPS_GET_AS_TEXT,
                                APPS_GET_TITLE  | APPS_GET_APP     | APPS_GET_APPVER  | APPS_GET_DISTRIB   | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                                APPS_GET_OSVER  | APPS_GET_EMUVER  | APPS_GET_EMUCAPS | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                                APPS_GET_COMPAT | APPS_GET_AS_TEXT | APPS_GET_AS_ID   | APPS_GET_AS_ICON,
                                $sortkey, $sortasc, $i, $n);

    if ($myReports && (count($myReports) > 0)) {
      HtmlSendReportList($myReports, $loggedin, false);
    } else {
      echo('<h2 class="normal">No compatibility reports were submitted by the given user</h2>');
    }
  } else {
   echo('TODO: stuff');
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
