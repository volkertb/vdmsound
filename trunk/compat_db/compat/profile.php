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
<?php HtmlBeginHeader(); ?>
<?php HtmlSendUserInfo($loggedin); ?><hr size="1" noshade>
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

<center>

<?php
  if ($profile) {
    if (!$success) {
      $color = '#7f4f4f';
      $text = ErrFormatError($error) . ' (' . $error . ').';
    } else {
      $color = '#4f7f4f';
      $text = 'Your profile information has been updated';
    }
  } else {
    $color = '#5f5f5f';
    $text = 'Update Personal Information';
  }

  echo('<p><table border="1" cellspacing="0" cellpadding="5" frame="hsides" width="50%"><tr align="center" bgcolor="' . $color . '"><td>' . $text . '</td></tr></table></p>');
?>

<form method="post" action="<?php echo($SCRIPT_NAME); ?>"><table border="0" cellspacing="2" cellpadding="3" width="100%">
  <tr><td align="right" width="50%">User:</td><td align="left" width="50%"><?php echo($userinfo['name']); ?></td></tr>
  <tr><td align="right">Created:</td><td align="left"><?php echo(date('D M j Y', strtotime($userinfo['time_created']))); ?></td></tr>
  <tr><td align="right">Last logged in:</td><td align="left"><?php echo(date('D M j Y', strtotime($userinfo['time_lastlogin']))); ?></td></tr>
  <tr><td align="right"><?php echo(HtmlHighlightError('Full name:', $error == E_AUTH_INVALID_FULL_NAME)); ?></td><td align="left"><input type="text" size="24" name="fullname" value="<?php echo($fullname); ?>" maxlength="48"></td></tr>
  <tr><td align="right"><?php echo(HtmlHighlightError('E-mail:', $error == E_AUTH_INVALID_EMAIL)); ?></td><td align="left"><input type="text" size="24" name="email" value="<?php echo($email); ?>" maxlength="64"></td></tr>
  <tr><td align="center" colspan="2"><input type="submit" value="Update">&nbsp;<input type="reset" value="Reset"></td></tr>
</table></form>

<br>
  
<?php
  if ($password) {
    if (!$success) {
      $color = '#7f4f4f';
      $text = ErrFormatError($error) . ' (' . $error . ').';
    } else {
      $color = '#4f7f4f';
      $text = 'Your password has been changed';
    }
  } else {
    $color = '#5f5f5f';
    $text = 'Change Password';
  }

  echo('<p><table border="1" cellspacing="0" cellpadding="5" frame="hsides" width="50%"><tr align="center" bgcolor="' . $color . '"><td>' . $text . '</td></tr></table></p>');
?>

<form method="post" action="<?php echo($SCRIPT_NAME); ?>"><table border="0" cellspacing="2" cellpadding="3" width="100%">
  <tr><td align="right" width="50%"><?php echo(HtmlHighlightError('Old password:', $error == E_AUTH_NO_SUCH_USER_PWD)); ?></td><td align="left" width="50%"><input type="password" size="12" name="oldpwd" value="" maxlength="16"></td></tr>
  <tr><td align="right"><?php echo(HtmlHighlightError('New password:', $error == E_AUTH_INVALID_PASSWORD)); ?></td><td align="left"><input type="password" size="12" name="newpwd1" value="" maxlength="16"></td></tr>
  <tr><td align="right"><?php echo(HtmlHighlightError('Confirm new password:', $error == E_AUTH_PASSWORD_MISMATCH)); ?></td><td align="left"><input type="password" size="12" name="newpwd2" value="" maxlength="16"></td></tr>
  <tr><td align="center" colspan="2"><input type="submit" value="Change">&nbsp;<input type="reset" value="Clear"></td></tr>
</table></form>

</center>

<?php HtmlBeginFooter(); ?>
<?php HtmlSendLastModified(); ?>
<?php HtmlEndPage(); ?>
