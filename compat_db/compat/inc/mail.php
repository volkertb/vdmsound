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

    $subj  = "DOS Compatibility Database Account Registration\n";
    $text  = "Thank you for registering on the DOS Compatibility Database web site.\n";
    $text .= "In order to complete your registration, visit the following url:\n\n";
    $text .= "http://" . $SERVER_NAME . $SCRIPT_NAME . "?action=activate&username=" . $username . "&hash=" . $hash . "\n\n";
    $text .= "Enjoy.\n";

    if (mail($mailto, $subj, $text)) {
      return true;
    } else {
      ErrSetLastError(E_MAIL_SENDMAIL);
      return false;
    }
  }
?>
