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

<p><center><font size="4"><b><?php
  if ($success) {
    echo("You have been logged out.");
  } else {
    echo("You were not logged in.");
  }
?></b></font></p>
<p><b><?php echo(HtmlMakeLink('Login', 'login.php')); ?></b> | <?php echo(HtmlMakeLink('Back', $HTTP_REFERER)); ?></center></p>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
