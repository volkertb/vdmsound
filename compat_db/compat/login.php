<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  if (isset($user, $password) && !isset($error)) {
    SessionStart();

    if (AuthLogin($user, $password)) {
      AuthUpdateLastLogin();
      if (!isset($redirect)) {
        $redirect = 'index.php';
      }
    } else {
      $redirect = $SCRIPT_NAME . '?user=' . $user . '&error=' . ErrGetLastError() . '&redirect=' . $redirect;
    }

    header('Location: ' . $redirect);
    exit;
  }
?>

<?php HtmlBeginPage('Log Into the DOS Compatibility Database'); ?>
<?php HtmlBeginHeader(); ?>
<?php HtmlBeginBody(); ?>

<?php
  if (isset($error)) {
    $style = 'error';
    $text  = ErrFormatError($error) . ' (' . $error . ').';
  } else {
    $style = 'normal';
    $text  = 'Type in your username and password';
  }

  echo('<h1 class="' . $style . '">' . $text . '</h1>');

  echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
  echo('<tr><td align="right" width="50%"><table border="0" cellspacing="2" cellpadding="3">');
  echo('<tr><td align="right">Username:</td><td><input type="text" size="16" name="user" value="' . $user . '" maxlength="12"></td></tr>');
  echo('<tr><td align="right">Password:</td><td><input type="password" size="16" name="password" maxlength="48"></td></tr>');
  echo('<tr><td align="right" colspan="2"><input type="submit" value="Login">&nbsp;<input type="reset" value="Clear"></td></tr>');
  echo('</table></td><td align="left" width="50%"><ul>');
  echo('<li>' . HtmlMakeLink('Forgot your password?', 'account.php?action=forgot') );
  echo('<li>' . HtmlMakeLink('Need to create an account?', 'account.php?action=create') );
  echo('<br><div style="margin-top: 5">Support:</div><a href="mailto:ntvdm@hotmail.com">ntvdm@hotmail.com</a>');
  echo('</ul></td></tr></table><input type="hidden" name="redirect" value="' . $redirect . '"></form>');
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
