<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/mail.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();
?>

<?php HtmlBeginPage('Account Management'); ?>
<?php HtmlBeginBody(); ?>

<?php $userinfo = AuthGetUserInformation(); ?>

<?php
  if ($action == 'create') {
    if (isset($username, $fullname, $email, $newpwd1, $newpwd2)) {
      $tried   = true;
      $success = AuthCreateUser($username, $fullname, $email, $newpwd1, $newpwd2);

      if ($success && !MailSendActivationCode($email, $username, AuthGetVerificationHash($username))) {
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
      echo('<h2 class="normal">Your account has been created.</h2>');
      echo('<p align="center">You will shortly receive an e-mail containing your activation code.</p>');
      echo('<p align="center"><b>' . HtmlMakeLink('Activate', $SCRIPT_NAME, Array('action=activate')) . '</b>&nbsp;| ' . HtmlMakeLink('Main', 'index.php') . '</p>');
    } else {
      if ($tried && !$success) {
        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      } else {
        $style = 'normal';
        $text  = 'Create an account';
      }

      echo('<h1 class="' . $style . '">' . $text . '</h1>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User ID:', ($error == E_AUTH_INVALID_USER_NAME) || ($error == E_AUTH_INUSE_NAME)) . '</td><td align="left" width="50%"><input class="flat" type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Full name:', $error == E_AUTH_INVALID_FULL_NAME) . '</td><td align="left"><input class="flat" type="text" size="24" name="fullname" value="' . $fullname . '" maxlength="48"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('E-mail:', $error == E_AUTH_INVALID_EMAIL) . '</td><td align="left"><input class="flat" type="text" size="24" name="email" value="' . $email. '" maxlength="64"></td></tr>');
      echo('<tr><td align="right"></td><td align="left"><table border="0" cellspacing="0" cellpadding="5" width="50%"><tr class="note"><td><b>Please enter a valid e-mail address.</b><br>You must provide a valid e-mail address before your account can be activated.</td></tr></table></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Password:', $error == E_AUTH_INVALID_PASSWORD) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd1" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Confirm password:', $error == E_AUTH_PASSWORD_MISMATCH) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd2" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Create">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table>');
      HtmlFormSendSID();
      echo('<input type="hidden" name="action" value="create"></form>');
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
      echo('<h2 class="normal">Your account has been activated.</h2>');
      echo('<p align="center"><b>' . HtmlMakeLink('Login', 'login.php') . '</b>&nbsp;| ' . HtmlMakeLink('Main', 'index.php') . '</p>');
    } else {
      if ($tried && !$success) {
        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      } else {
        $style = 'normal';
        $text  = 'Activate your account';
      }

      echo('<h1 class="' . $style . '">' . $text . '</h1>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User ID:', $error == E_AUTH_INVALID_USER_NAME) . '</td><td align="left" width="50%"><input class="flat" type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Activation code:', $error == E_AUTH_INVALID_HASH) . '</td><td align="left"><input class="flat" type="text" size="24" name="hash" value="' . $hash . '" maxlength="32"></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Activate">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table>');
      HtmlFormSendSID();
      echo('<input type="hidden" name="action" value="activate"></form>');
    }
  } else if ($action == 'forgot') {
    if (isset($username)) {
      $tried   = true;

      if ((($userid = AuthGetUserId($username)) > 0) &&
          ($userinfo = AuthGetUserInformation($userid)) &&
          (MailSendPasswordResetCode($userinfo['email'], $userinfo['name'], AuthGetVerificationHash($userinfo['name']))))
      {
        $success = true;
      } else {
        $success = false;
        $error = ErrGetLastError();
      }
    } else {
      $tried   = false;
    }

    if ($tried && $success) {
      echo('<h2 class="normal">Your request has been processed.</h2>');
      echo('<p align="center">You will shortly receive an e-mail containing your password reset verification code needed in <cite>step 2</cite>.</p>');
      echo('<p align="center"><b>' . HtmlMakeLink('Reset password (step 2)', $SCRIPT_NAME, Array('action=pwreset')) . '</b>&nbsp;| ' . HtmlMakeLink('Main', 'index.php') . '</p>');
    } else {
      if ($tried && !$success) {
        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      } else {
        $style = 'normal';
        $text  = 'Reset password (step 1)';
      }

      echo('<h1 class="' . $style . '">' . $text . '</h1>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User ID:', ($error == E_AUTH_INVALID_USER_NAME) || ($error == E_AUTH_INUSE_NAME)) . '</td><td align="left" width="50%"><input class="flat" type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right"></td><td align="left"><table border="0" cellspacing="0" cellpadding="5" width="50%"><tr class="note"><td>Provide the same user ID that you would normally enter at the database\'s login prompt.</td></tr></table></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Proceed">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table>');
      HtmlFormSendSID();
      echo('<input type="hidden" name="action" value="forgot"></form>');
    }
  } else if ($action == 'pwreset') {
    if (isset($username, $hash, $newpwd1, $newpwd2)) {
      $tried   = true;
      $success = AuthResetUserPassword($username, $hash, $newpwd1, $newpwd2);
      $error   = ErrGetLastError();
    } else {
      $tried   = false;
    }

    if ($tried && $success) {
      echo('<h2 class="normal">Your password has been changed.</h2>');
      echo('<p align="center"><b>' . HtmlMakeLink('Login', 'login.php') . '</b>&nbsp;| ' . HtmlMakeLink('Main', 'index.php') . '</p>');
    } else {
      if ($tried && !$success) {
        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      } else {
        $style = 'normal';
        $text  = 'Reset password (step 2)';
      }

      echo('<h1 class="' . $style . '">' . $text . '</h1>');

      echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
      echo('<tr><td align="right" width="50%">' . HtmlHighlightError('User ID:', $error == E_AUTH_INVALID_USER_NAME) . '</td><td align="left" width="50%"><input class="flat" type="text" size="12" name="username" value="' . $username . '" maxlength="12"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Activation code:', $error == E_AUTH_INVALID_HASH) . '</td><td align="left"><input class="flat" type="text" size="24" name="hash" value="' . $hash . '" maxlength="32"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('New password:', $error == E_AUTH_INVALID_PASSWORD) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd1" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="right">' . HtmlHighlightError('Confirm new password:', $error == E_AUTH_PASSWORD_MISMATCH) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd2" value="" maxlength="16"></td></tr>');
      echo('<tr><td align="center" colspan="2"><input type="submit" value="Set password">&nbsp;<input type="reset" value="Reset"></td></tr>');
      echo('</table>');
      HtmlFormSendSID();
      echo('<input type="hidden" name="action" value="pwreset"></form>');
    }
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
