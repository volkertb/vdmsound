<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/error.php');

  ////////////////////////////////////////////////////////////////////////////
  // Methods
  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function MailSendActivationCode($mailto, $username, $hash) {
    global $SERVER_NAME, $SCRIPT_NAME;
    ErrSetLastError();

    if (strlen($hash) != 32) {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }

    $subj  = "DOS Compatibility Database Account Registration";
    $text  = "Thank you for registering on the DOS Compatibility Database web site.\n\n";
    $text .= "In order to complete your registration, visit the following URL:\n\n";
    $text .= "http://" . $SERVER_NAME . $SCRIPT_NAME . "?action=activate&usrname=" . $username . "&hash=" . $hash . "\n\n";
    $text .= "Enjoy.\n";

    if (mail($mailto, $subj, $text)) {
      return true;
    } else {
      ErrSetLastError(E_MAIL_SENDMAIL);
      return false;
    }
  }

  //
  //
  //
  function MailSendPasswordResetCode($mailto, $username, $hash) {
    global $SERVER_NAME, $SCRIPT_NAME;
    ErrSetLastError();

    if (strlen($hash) != 32) {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }

    $subj  = "DOS Compatibility Database Password Reset Verification";
    $text  = "Someone (presumably you) on the DOS Compatibility Database site\n";
    $text .= "requested a password change through email verification. If this\n";
    $text .= "was not you, ignore this message and nothing will happen.\n\n";
    $text .= "If you requested this verification, visit the following URL:\n\n";
    $text .= "http://" . $SERVER_NAME . $SCRIPT_NAME . "?action=pwreset&usrname=" . $username . "&hash=" . $hash . "\n\n";
    $text .= "Good luck.\n";

    if (mail($mailto, $subj, $text)) {
      return true;
    } else {
      ErrSetLastError(E_MAIL_SENDMAIL);
      return false;
    }
  }
?>
