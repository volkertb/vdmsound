<?php
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();

  if (AuthLogin()) {
    $success = true;
    AuthLogout();
  } else {
    $success = false;
  }

  SessionDestroy();
?>

<?php HtmlBeginPage('Log Out of the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader(); ?>
<?php HtmlBeginBody(); ?>

<?php
  if ($success) {
    echo('<h2 class="normal">You have been logged out.</h2>');
  } else {
    echo('<h2 class="normal">You were not logged in.</h2>');
  }

  echo('<p align="center"><b>' . HtmlMakeLink('Login', 'login.php') . '</b> | ' . HtmlMakeLink('Back', $HTTP_REFERER) );
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
