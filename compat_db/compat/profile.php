<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();

  if (!$loggedin = AuthLogin()) {
    header('Location: login.php?redirect=' . $SCRIPT_NAME);
    exit;
  }
?>

<?php HtmlBeginPage('Edit Your User Profile'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  $userinfo = AuthGetUserInformation();

  if (isset($fullname, $email)) {
    $profile  = true;
    $success  = AuthUpdateProfile($fullname, $email);
    $error    = ErrGetLastError();
  } else {
    $fullname = $userinfo['fullname'];
    $email    = $userinfo['email'];
    $profile  = false;
  }

  if (isset($oldpwd, $newpwd1, $newpwd2)) {
    $password = true;
    $success  = AuthChangePassword($oldpwd, $newpwd1, $newpwd2);
    $error    = ErrGetLastError();
  } else {
    $password = false;
  }
?>

<?php
  if ($profile) {
    if (!$success) {
      $style = 'error';
      $text  = ErrFormatError($error) . ' (' . $error . ')';
    } else {
      $style = 'success';
      $text  = 'Your profile information has been updated';
    }
  } else {
    $style = 'normal';
    $text  = 'Update Personal Information';
  }

  echo('<h1 class="' . $style . '">' . $text . '</h1>');

  echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
  echo('<tr><td align="right" width="50%">User ID:</td><td align="left" width="50%">' . $userinfo['name'] . '</td></tr>');
  echo('<tr><td align="right">Created:</td><td align="left">' . date('D M j Y', strtotime($userinfo['time_created'])) . '</td></tr>');
  echo('<tr><td align="right">Last logged in:</td><td align="left">' . date('D M j Y', strtotime($userinfo['time_lastlogin'])) . '</td></tr>');
  echo('<tr><td align="right">' . HtmlHighlightError('Full name:', $error == E_AUTH_INVALID_FULL_NAME) . '</td><td align="left"><input class="flat" type="text" size="24" name="fullname" value="' . $fullname . '" maxlength="48"></td></tr>');
  echo('<tr><td align="right">' . HtmlHighlightError('E-mail:', $error == E_AUTH_INVALID_EMAIL) . '</td><td align="left"><input class="flat" type="text" size="24" name="email" value="' . $email . '" maxlength="64"></td></tr>');
  echo('<tr><td align="center" colspan="2"><input type="submit" value="Update">&nbsp;<input type="reset" value="Reset"></td></tr>');
  HtmlFormSendSID();
  echo('</table></form>');
?>

<br>

<?php
  if ($password) {
    if (!$success) {
      $style = 'error';
      $text  = ErrFormatError($error) . ' (' . $error . ')';
    } else {
      $style = 'success';
      $text  = 'Your password has been changed';
    }
  } else {
    $style = 'normal';
    $text  = 'Change Password';
  }

  echo('<h1 class="' . $style . '">' . $text . '</h1>');


  echo('<form method="post" action="' . $SCRIPT_NAME . '"><table border="0" cellspacing="2" cellpadding="3" width="100%">');
  echo('<tr><td align="right" width="50%">' . HtmlHighlightError('Old password:', $error == E_AUTH_NO_SUCH_USER_PWD) . '</td><td align="left" width="50%"><input class="flat" type="password" size="12" name="oldpwd" value="" maxlength="16"></td></tr>');
  echo('<tr><td align="right">' . HtmlHighlightError('New password:', $error == E_AUTH_INVALID_PASSWORD) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd1" value="" maxlength="16"></td></tr>');
  echo('<tr><td align="right">' . HtmlHighlightError('Confirm new password:', $error == E_AUTH_PASSWORD_MISMATCH) . '</td><td align="left"><input class="flat" type="password" size="12" name="newpwd2" value="" maxlength="16"></td></tr>');
  echo('<tr><td align="center" colspan="2"><input type="submit" value="Change">&nbsp;<input type="reset" value="Clear"></td></tr>');
  HtmlFormSendSID();
  echo('</table></form>');

?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
