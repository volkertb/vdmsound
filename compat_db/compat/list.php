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
  if (isset($reportid)) {
    $myReports = AppsGetReports($reportid, NULL, NULL, true,
                                APPS_GET_USER  | APPS_GET_COMMENT | APPS_GET_AS_TEXT,
                                APPS_GET_TITLE | APPS_GET_APPVER  | APPS_GET_DISTRIB | APPS_GET_AS_TEXT,
                                APPS_GET_OSVER | APPS_GET_EMUVER  | APPS_GET_AS_TEXT,
                                APPS_GET_CMIDI | APPS_GET_CSB     | APPS_GET_CADLIB  | APPS_GET_CJOYSTICK | APPS_GET_AS_TEXT,
                                NULL, false, 0, 10);

    echo('<table border="0" cellspacing="2" cellpadding="5">');

    if (count($myReports) > 0) {
      foreach ($myReports as $myReport) {
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Title:</td>');
        echo('<td align="left">' . $myReport['title_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Version:</td>');
        echo('<td align="left">' . $myReport['appver_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Distributed as:</td>');
        echo('<td align="left">' . $myReport['distrib_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Operating system used:</td>');
        echo('<td align="left">' . $myReport['osver_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Emulator used:</td>');
        echo('<td align="left">' . $myReport['emuver_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Submitted by:</td>');
        echo('<td align="left">' . $myReport['user_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Comment:</td>');
        echo('<td align="left">' . $myReport['comment_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">MIDI compatibility:</td>');
        echo('<td align="left">' . $myReport['cmidi_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">SoundBlaster compatibility:</td>');
        echo('<td align="left">' . $myReport['csb_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">AdLib compatibility:</td>');
        echo('<td align="left">' . $myReport['cadlib_text'] . '</td></tr>');
        echo('<tr bgcolor="#5f5f5f" valign="middle"><td align="right">Joystick compatibility:</td>');
        echo('<td align="left">' . $myReport['cjoystick_text'] . '</td></tr>');
      }
    } else {
      echo('Error');
    }

    echo('</table>');

  } else {
   echo('blank');
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
