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

<center>

<?php
  if (isset($error)) {
    $color = '#7f4f4f';
    $text = ErrFormatError($error) . ' (' . $error . ').';
  } else {
    $color = '#5f5f5f';
    $text = 'Type in your username and password';
  }

  echo('<p><table border="1" cellspacing="0" cellpadding="5" frame="hsides" width="50%"><tr align="center" bgcolor="' . $color . '"><td>' . $text . '</td></tr></table></p>');
?>
  
<table border="0" cellspacing="2" cellpadding="3" width="100%"><tr valign="center">
  <td width="50%" align="right"><form method="post" action="<?php echo($SCRIPT_NAME); ?>"><table border="0" cellspacing="2" cellpadding="3">
    <tr><td align="right">Username:</td><td><input type="text" size="16" name="user" value="<?php echo($user); ?>" maxlength="12"></td></tr>
    <tr><td align="right">Password:</td><td><input type="password" size="16" name="password" maxlength="48"></td></tr>
    <tr><td align="right" colspan="2"><input type="submit" value="Login">&nbsp;<input type="reset" value="Clear"></td></tr>
  </table><input type="hidden" name="redirect" value="<?php echo($redirect); ?>"></form></td>
  <td width="50%" align="left"><p><ul>
    <li><a href="account.php?action=forgot">Forgot your password?</a>
    <li><a href="account.php?action=create">Need to create an account?</a><font size="1"><br><br></font>
    Support:<br><a href="mailto:ntvdm@hotmail.com">ntvdm@hotmail.com</a>
  </ul></p></td>
</tr></table>

</center>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
