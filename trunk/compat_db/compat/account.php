<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/mail.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();
?>

<?php HtmlBeginPage('Account Management'); ?>
<?php HtmlBeginHeader(); ?>
<?php HtmlBeginBody(); ?>

<?php $userinfo = AuthGetUserInformation(); ?>

<center>

<?php
  if ($action == 'create') {
    if (isset($username, $fullname, $email, $newpwd1, $newpwd2)) {
      $tried   = true;
      $success = AuthCreateUser($username, $fullname, $email, $newpwd1, $newpwd2);

      if ($success && !MailSendActivationCode($email, $username, AuthGetActivationHash($username))) {
        $success = false;
        $error = ErrGetLastError();

        AuthDeleteUser($username, $newpwd1, 0);  // rollback user creation
      } else {
        $error = ErrGetLastError();
      }
    } else {
      $tried   = false;
    }

    if ($tried && $success) {
      echo('<p><font size="4"><b>Your account has been created.</b></font></p>');
      echo('<p>You will shortly receive an e-mail containing your activation code.</p>');
      echo('<p><b>' . HtmlMakeLink('Activate', $SCRIPT_NAME . '?action=activate') . '</b></a> | ' . HtmlMakeLink('Main', 'index.php') . '</b>');
    } else {
      if ($tried && !$success) {
        $color = '#7f4f4f';
        $text = ErrFormatError($error) . ' (' . $error . ').';
      } else {
        $color = '#4f7f4f';
        $text = 'Create an account';
      }

      echo('<p><table border="1" cellspacing="0" cellpadding="5" frame="hsides" width="50%"><tr align="center" bgcolor="' . $color . '"><td>' . $text . '</td></tr></table></p>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User:', ($error == E_AUTH_INVALID_USER_NAME) || ($error == E_AUTH_INUSE_NAME)) . '</td><td align="left" width="50%"><input type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Full name:', $error == E_AUTH_INVALID_FULL_NAME) . '</td><td align="left"><input type="text" size="24" name="fullname" value="' . $fullname . '" maxlength="48"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('E-mail:', $error == E_AUTH_INVALID_EMAIL) . '</td><td align="left"><input type="text" size="24" name="email" value="' . $email. '" maxlength="64"></td></tr>');
      echo('<tr><td align="right"></td><td align="left"><table border="0" cellspacing="0" cellpadding="5" width="50%"><tr bgcolor="#7f7f5f"><td><font size="2"><b>Please enter a valid e-mail address.</b><br>You must provide a valid e-mail address before your account can be activated.</font></td></tr></table></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Password:', $error == E_AUTH_INVALID_PASSWORD) . '</td><td align="left"><input type="password" size="12" name="newpwd1" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Confirm password:', $error == E_AUTH_PASSWORD_MISMATCH) . '</td><td align="left"><input type="password" size="12" name="newpwd2" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Create">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table><input type="hidden" name="action" value="create"></form>');
    }
  } else if ($action == 'activate') {
    if (isset($username, $hash)) {
      $tried   = true;
      $success = AuthActivateUser($username, $hash);
      $error   = ErrGetLastError();
    } else {
      $tried   = false;
    }

    if ($tried && $success) {
      echo('<p><font size="4"><b>Your account has been activated.</b></font></p>');
      echo('<p><b>' . HtmlMakeLink('Login', 'login.php') . '</b></a> | ' . HtmlMakeLink('Main', 'index.php') . '</b>');
    } else {
      if ($tried && !$success) {
        $color = '#7f4f4f';
        $text = ErrFormatError($error) . ' (' . $error . ').';
      } else {
        $color = '#4f7f4f';
        $text = 'Activate your account';
      }

      echo('<p><table border="1" cellspacing="0" cellpadding="5" frame="hsides" width="50%"><tr align="center" bgcolor="' . $color . '"><td>' . $text . '</td></tr></table></p>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User:', $error == E_AUTH_INVALID_USER_NAME) . '</td><td align="left" width="50%"><input type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Activation code:', $error == E_AUTH_INVALID_HASH) . '</td><td align="left"><input type="text" size="24" name="hash" value="' . $hash . '" maxlength="32"></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Activate">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table><input type="hidden" name="action" value="activate"></form>');
    }
  } else if ($action == 'activate') {

  }
?>

</center>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
