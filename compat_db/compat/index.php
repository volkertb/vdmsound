<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');
  require_once('inc/apps.php');

  SessionStart();

  $loggedin = AuthLogin();
?>

<?php HtmlBeginPage('Welcome to the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  $globStats = AppsGetGlobalStats();
  $myReports = AppsGetReports(NULL, AuthGetUserId(), NULL, true,
                              APPS_GET_USER  | APPS_GET_AS_TEXT,
                              APPS_GET_TITLE | APPS_GET_APPVER | APPS_GET_DISTRIB | APPS_GET_AS_TEXT,
                              APPS_GET_OSVER | APPS_GET_EMUVER | APPS_GET_AS_TEXT,
                              0, 'updated', false, 0, 10);

  echo('<table border="0" cellspacing="0" cellpadding="7" width="100%" height="100%"><tr valign="top">');

  echo('<td align="left">');
  echo('xxx');

  echo('</td><td align="center">');
  echo('App = ' . $globStats['apps'] . '<br>');
  echo('Rep = ' . $globStats['reports']);

  echo('</td><td align="right">');

  echo('<table border="0" cellspacing="2" cellpadding="5">');

  if ($loggedin) {
    $hdr = 'My compatibility reports';
  } else {
    $hdr = 'Newest compatibility reports';
  }

  echo('<tr><td colspan="2"><h2>' . $hdr . '</h2></td></tr>');

  if (count($myReports) > 0) {
    $i = 0;

    foreach ($myReports as $myReport) {
      echo('<tr bgcolor="#5f5f5f" valign="middle"><td>');
      echo('<div style="font: 60%">' . date('D M j Y', strtotime($myReport['updated'])) . ' &nbsp; (contributed by <em>' . $myReport['user_text'] . '</em>)</div>');
      echo('<div style="font: serif; text-indent: 10"><b>' . $myReport['title_text'] . '</b> ' . $myReport['appver_text'] . ' <i>(' . $myReport['distrib_text'] . ')</i></div>');
      echo('<div style="font: 75% sans-serif; text-indent: 10; color: #bfbfbf">Tested under ' . $myReport['osver_text'] . ' using ' . $myReport['emuver_text'] . '</div>');

      echo('</td><td align="center"><div style="font: 75% sans-serif">');
      echo(HtmlMakeLink('View', 'list.php', Array('reportid' => $myReport['report_id'])));

      if ($loggedin) {
        echo('<br>' . HtmlMakeLink('Edit', 'edit.php', Array('reportid' => $myReport['report_id'])));
        echo('<br>' . HtmlMakeLink('Delete', 'edit.php', Array('reportid' => $myReport['report_id'])));
      }

      echo('</div></td></tr>');
    }
  } else {
    echo('<tr><td colspan="2"><h3>You have not submitted any compatibility reports</h3></td></tr>');
  }

  echo('</table>');

  echo('</td></tr></table>');
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
