<?php
  ////////////////////////////////////////////////////////////////////////////
  // Constants
  ////////////////////////////////////////////////////////////////////////////

  define('E_NOERROR', 0);

  define('E_AUTH_UNKNOWN_USER_ID',   1000);
  define('E_AUTH_UNKNOWN_USER_NAME', 1001);
  define('E_AUTH_INVALID_USER_ID',   1100);
  define('E_AUTH_INVALID_USER_NAME', 1101);
  define('E_AUTH_NO_SUCH_USER_PWD',  1102);
  define('E_AUTH_INVALID_FULL_NAME', 1110);
  define('E_AUTH_INVALID_EMAIL',     1111);
  define('E_AUTH_INVALID_PASSWORD',  1120);
  define('E_AUTH_PASSWORD_MISMATCH', 1121);
  define('E_AUTH_INUSE_NAME',        1200);
  define('E_AUTH_INVALID_HASH',      1210);

  define('E_MYSQL_CANT_CONNECT',     2000);
  define('E_MYSQL_CANT_DISCONNECT',  2001);
  define('E_MYSQL_NO_CONNECTION',    2002);

  define('E_MYSQL_CANT_SEL_DB',      2100);
  define('E_MYSQL_CANT_QUERY_DB',    2101);

  define('E_MAIL_SENDMAIL',          2200);

  $err_codes = Array(
    E_AUTH_UNKNOWN_USER_ID   => 'Unknown user ID',
    E_AUTH_UNKNOWN_USER_NAME => 'Unknown user name',
    E_AUTH_INVALID_USER_ID   => 'Invalid user ID',
    E_AUTH_INVALID_USER_NAME => 'Invalid user name',
    E_AUTH_NO_SUCH_USER_PWD  => 'Invalid user name or password',
    E_AUTH_INVALID_FULL_NAME => 'Invalid full name',
    E_AUTH_INVALID_EMAIL     => 'Invalid e-mail address',
    E_AUTH_INVALID_PASSWORD  => 'Invalid password',
    E_AUTH_PASSWORD_MISMATCH => 'The passwords do not match',
    E_AUTH_INUSE_NAME        => 'The specified user already exists',
    E_AUTH_INVALID_HASH      => 'Invalid confirmation code',
    E_MYSQL_CANT_CONNECT     => 'Unable to connect to server',
    E_MYSQL_CANT_DISCONNECT  => 'Unable to disconnect from server',
    E_MYSQL_NO_CONNECTION    => 'Not connected to server',
    E_MYSQL_CANT_SEL_DB      => 'Unable to switch database',
    E_MYSQL_CANT_QUERY_DB    => 'Unable to query database',
    E_MAIL_SENDMAIL          => 'Unable to send mail'
  );
    

  ////////////////////////////////////////////////////////////////////////////
  // Methods
  ////////////////////////////////////////////////////////////////////////////

  //
  // Sets the last error code
  //
  function ErrSetLastError($errorcode = E_NOERROR) {
    global $err_lasterrorcode;
    $err_lasterrorcode = $errorcode;
  }

  //
  // Gets the last error code
  //
  function ErrGetLastError() {
    global $err_lasterrorcode;
    return $err_lasterrorcode;
  }

  //
  // Converst error code into human-readable string
  //
  function ErrFormatError($errorcode) {
    global $err_lasterrorcode, $err_codes;

    if ($errorcode == E_NOERROR)
      return '[No error]';

    if (is_null($errorcode) || is_null($err_codes[$errorcode]))
      return '[Unknown error]';

    return $err_codes[$errorcode];
  }
?>
