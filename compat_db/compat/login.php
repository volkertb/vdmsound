<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  if (isset($usrname, $password) && !isset($error)) {
    SessionStart();

    if (AuthLogin($usrname, $password)) {
      AuthUpdateLastLogin();
      if (!isset($redirect) || (strlen($redirect) == 0)) {
        $redirect = HtmlMakeURL('index.php');
      } else {
        $redirect = HtmlMakeURL($redirect);
      }
    } else {
      $redirect = HtmlMakeURL($SCRIPT_NAME, Array('usrname' => $usrname, 'error' => ErrGetLastError(), 'redirect' => $redirect));
    }

    header('Location: ' . $redirect);
    exit;
  }
?>

<?php HtmlBeginPage('Log Into the DOS Compatibility Database'); ?>
<?php HtmlBeginBody(); ?>

<?php
  if (isset($error)) {
    $style = 'error';
    $text  = ErrFormatError($error) . ' (' . $error . ')';
  } else {
    $style = 'normal';
    $text  = 'Type in your username and password';
  }

  echo('<h1 class="' . $style . '">' . $text . '</h1>');

  echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
  echo('<tr valign="middle"><td align="right" width="50%"><table border="0" cellspacing="2" cellpadding="3">');
  echo('<tr><td align="right">User ID:</td><td><input class="flat1" type="text" size="16" name="usrname" value="' . $usrname . '" maxlength="12"></td></tr>');
  echo('<tr><td align="right">Password:</td><td><input class="flat1" type="password" size="16" name="password" maxlength="48"></td></tr>');
  echo('<tr><td align="right" colspan="2"><input type="submit" class="flat2" value="Login">&nbsp;<input type="reset" class="flat2" value="Clear"></td></tr>');
  echo('</table></td><td align="left" width="50%"><ul>');
  echo('<li>' . HtmlMakeLink('Forgot your password?', 'account.php', Array('action' => 'forgot', 'usrname' => $usrname)) );
  echo('<li>' . HtmlMakeLink('Need to create an account?', 'account.php', Array('action' => 'create')) );
  echo('<br><div style="margin-top: 0.5em">Support:</div><a href="mailto:ntvdm@hotmail.com">ntvdm@hotmail.com</a>');
  echo('</ul></td></tr></table>');
  HtmlFormSendSID();
  echo('<input type="hidden" name="redirect" value="' . $redirect . '"></form>');
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
