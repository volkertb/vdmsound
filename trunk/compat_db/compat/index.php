<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');
  require_once('inc/news.php');
  require_once('inc/apps.php');

  SessionStart();

  $loggedin = AuthLogin();
?>

<?php HtmlBeginPage('Welcome to the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  $globStats = AppsGetGlobalStats();
  $theNews   = NewsGetItems('updated', false, 0, 5);
  $myReports = AppsGetReports(NULL, AuthGetUserId(), NULL, true,
                              APPS_GET_USER  | APPS_GET_AS_TEXT,
                              APPS_GET_TITLE | APPS_GET_APPVER | APPS_GET_DISTRIB | APPS_GET_AS_TEXT,
                              APPS_GET_OSVER | APPS_GET_EMUVER | APPS_GET_AS_TEXT,
                              0, 'updated', false, 0, 5);

  echo('<table border="0" cellspacing="0" cellpadding="7" width="100%"><tr valign="top">');

  //
  // The "titles" section
  //

  echo('<td align="left" width="15%">');

  echo('<h1 class="normal"><nobr> Apps </nobr></h1>');
  echo('<table border="0" cellspacing="0" cellpadding="5" width="100%">');
  echo('<tr class="opaque1"><td class="opaque1_bevel"><small>Currently supporting ' . $globStats['apps'] . ' DOS applications with ' . $globStats['reports'] . ' compatibility reports!</small></td></tr>');

  echo('<tr><td height="2"></td></tr>');

  $slices = 9;
  $index = Array('#');

  for ($i = ord('A'); $i <= ord('Z'); $i++) {
    array_push($index, chr($i));
  }

  echo('<tr class="opaque1"><td class="opaque1_bevel"><div style="margin-bottom: 0.5em">Browse:</div>');
  echo('<table border="0" cellspacing="0" cellpadding="1" width="100%" style="font-family: monospace">');

  for ($i = 0; $i < count($index); $i++) {
    if (($i % $slices) == 0) {
      echo('<tr>');
    }

    echo('<td align="center">' . HtmlMakeLink($index[$i], 'search.php', Array('byletter' => $index[$i])) . '</td>');

    if ((($i % $slices) == ($slices - 1)) || ($i == (count($index) - 1))) {
      echo('</tr>');
    }
  }

  echo('</table></td></tr>');

  echo('<tr><td height="2"></td></tr>');

  echo('<form method="get" action="search.php"><tr class="opaque1" valign="top"><td class="opaque1_bevel" align="right">Search:&nbsp;<input class="flat1" type="text" size="12" name="query" value=""><br>');
  echo('<label accesskey="x"><input type="checkbox" name="exact" value="yes">Exact match</label>&nbsp;<input type="submit" class="flat2" value="Go!">');
  HtmlFormSendSID();
  echo('</td></tr></form>');

  echo('</table>');

  //
  // The "news" section
  //

  echo('</td><td align="center" width="45%">');
  echo('<h1 class="normal"><nobr> What\'s new </nobr></h1>');

  if ($theNews && (count($theNews) > 0)) {
    HtmlSendNewsList($theNews);
  } else {
    echo('<h2 class="normal">No news items are available at this time</h2>');
  }

  //
  // The "reports" section
  //

  if ($loggedin) {
    $hdr = 'My compatibility reports';
  } else {
    $hdr = 'Latest compatibility reports';
  }

  echo('</td><td align="right" width="40%">');
  echo('<h1 class="normal"><nobr> ' . $hdr . ' </nobr></h1>');

  if ($myReports && (count($myReports) > 0)) {
    HtmlSendReportList($myReports, $loggedin, true);
  } else {
    if ($loggedin) {
      $errMsg = 'You have not yet submitted any compatibility reports';
    } else {
      $errMsg = 'No compatibility reports are available at this time';
    }

    echo('<h2 class="normal">' . $errMsg . '</h2>');
  }

  echo('</table>');
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
