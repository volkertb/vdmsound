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

  $navtrk = true;
  $errmsg = 'Unknown error';

  if (isset($reportid)) {
    $navtrk = false;
    $errmsg = 'No compatibility report matching the given ID exists';
  } else if (isset($appid)) {
    if (!isset($sortkey)) $sortkey = 'updated';
    if (!isset($sortasc)) $sortasc = false;

    $errmsg = 'No compatibility reports exist for the given application ID';
  } else if (isset($userid)) {
    if (strlen($userid) < 1) $userid = NULL; // KLUDGE: lists report for all users if no user is specified

    if (!isset($sortkey)) $sortkey = 'title_text';
    if (!isset($sortasc)) $sortasc = true;

    $errmsg = 'No compatibility reports were submitted by the given user';
  }

  $myReports = AppsGetReports($reportid, $userid, $appid, true,
                              APPS_GET_USER   | APPS_GET_COMMENT | APPS_GET_AS_ID   | APPS_GET_AS_TEXT,
                              APPS_GET_TITLE  | APPS_GET_APP     | APPS_GET_APPVER  | APPS_GET_DISTRIB   | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                              APPS_GET_OSVER  | APPS_GET_EMUVER  | APPS_GET_EMUCAPS | APPS_GET_AS_TEXT | APPS_GET_AS_ICON,
                              APPS_GET_COMPAT | APPS_GET_AS_TEXT | APPS_GET_AS_ID   | APPS_GET_AS_ICON,
                              $sortkey, $sortasc, $i, $n, $navtrk);

  if ($myReports && (count($myReports) > 0)) {
    HtmlSendReportList($myReports, $loggedin, false, HtmlMakeResultsInfo($i, $n, AppsGetLastNumRows(), 'Reports', $REQUEST_URI), $REQUEST_URI, $sortkey, $sortasc);
  } else {
    echo('<h2 class="normal">' . $errmsg . '</h2>');
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
