<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();

  $loggedin = AuthLogin();
?>

<?php HtmlBeginPage('Browse the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader(); ?>
<?php HtmlSendUserInfo($loggedin); ?><hr size="1" noshade>
<?php HtmlBeginBody(); ?>

Bla bla.
Text.

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
