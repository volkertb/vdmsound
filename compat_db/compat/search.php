<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');
  require_once('inc/apps.php');

  SessionStart();

  $loggedin = AuthLogin();
?>

<?php HtmlBeginPage('Search the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  if (!isset($i)) $i = 0;
  if (!isset($n)) $n = 25;

  if (isset($byletter)) {
    if ($byletter == '#') {
      $appquery = '^[^a-zA-Z]';
      $queryOp  = 'RLIKE';
      $hdrText  = '0 - 9';
    } else {
      $appquery = $byletter . '%';
      $queryOp  = 'LIKE';
      $hdrText  = '- ' . substr($byletter, 0, 1) . ' -';
    }

    $errmsg = 'No applications whose names begin with this letter were found';
  } else if (isset($query)) {
    $hdrText  = 'Search results';
    $errmsg = 'No applications whose names match the query string were found';

    if ($exact == 'yes') {
      $appquery = $query;
      $queryOp  = '=';
    } else {
      $appquery = Array();
      $queryOp  = 'LIKE';

      foreach (explode(' ', $query) as $term) {
        array_push($appquery, '%' . $term . '%');
      }
    }
  } else {
    echo('Advanced search form');
  }

  if (isset($appquery, $queryOp)) {
    $myApps = AppsGetByName($appquery, $queryOp, true, 'title_text', true, $i, $n);

    echo('<h1 class="normal">' . $hdrText . '</h1>');

    if ($myApps && (count($myApps) > 0)) {
      HtmlSendAppsList($myApps);
    } else {
      echo('<h2 class="normal">' . $errmsg . '</h2>');
    }
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
