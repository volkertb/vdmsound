<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/session.php');
  require_once('inc/error.php');
  require_once('inc/mysql.php');

  ////////////////////////////////////////////////////////////////////////////
  // One-time initial setup
  ////////////////////////////////////////////////////////////////////////////

  SessionStart();
  MysqlConnect();

  if (!session_is_registered('auth_userid') ||
      !session_is_registered('auth_pwhash'))
  {
    session_register('auth_userid');  // user id (numeric)
    session_register('auth_pwhash');  // password md5 hash
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Validation
  ////////////////////////////////////////////////////////////////////////////

  function AuthIsValidUsername($username) {
    return eregi('^[0-9A-Z_]+$', $username);       // accept alpha-numeric only
  }

  function AuthIsValidFullName($fullname) {
    return eregi('^[ \,\.0-9A-Z_]+$', $fullname);  // accept alpha-numeric only with some punctuation
  }

  function AuthIsValidEmail($email) {
    return eregi('^[\!\%\&\-\.0-9A-Z_]+@[\&\-\.0-9A-Z_]+$', $email); // accept alpha-numeric only with some punctuation
  }

  function AuthIsValidHash($hash) {
    return eregi('^[0-9A-F]+$', $hash);            // accept hex only
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Miscellaneous
  ////////////////////////////////////////////////////////////////////////////

  function AuthSRand() {
    list($usec, $sec) = explode(' ', microtime());
    srand($usec * 100000 + $sec);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - General
  ////////////////////////////////////////////////////////////////////////////

  //
  // Retrieves the user id given a user name, or returns the logged-in user id
  //  if $username is NULL
  //
  // *************************************************************************
  // ** The user ID should not be used outside the server since it is not   **
  // ** validated by Auth functions and may be used for SQL code injection! **
  // *************************************************************************
  //
  function AuthGetUserId($username = NULL) {
    global $auth_userid;
    ErrSetLastError();

    if (is_null($username))
      return $auth_userid;

    $username = strtolower($username);

    if (!AuthIsValidUsername($username)) {   // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_USER_NAME);
      return NULL;
    }

    // Lookup the user id
    if (!($result = MysqlQuery('SELECT id FROM Users WHERE name="' . $username . '"')))
      return false;

    if (list($userid) = mysql_fetch_row($result)) {
      return $userid;
    } else {
      ErrSetLastError(E_AUTH_UNKNOWN_USER_NAME);
      return NULL;
    }
  }

  //
  // Retrieves the user display information given a user id
  //
  function AuthGetUserInformation($userid = NULL) {
    global $auth_userid;
    ErrSetLastError();

    if (is_null($userid))
      $userid = $auth_userid;

    if (!($result = MysqlQuery('SELECT name,type,time_created,DATE_FORMAT(time_updated,"%Y-%m-%d %H:%i%s"),time_lastlogin,fullname,email FROM Users WHERE id="' . $userid . '"')))
      return false;

    if ($userinfo = mysql_fetch_assoc($result)) {
      return $userinfo;
    } else {
      ErrSetLastError(E_AUTH_UNKNOWN_USER_ID);
      return false;
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Authentication
  ////////////////////////////////////////////////////////////////////////////

  //
  // Checks whether the passed username/password or cached userid/hash
  //   are valid
  //
  function AuthLogin($username = NULL, $password = NULL) {
    global $auth_userid, $auth_pwhash;
    ErrSetLastError();

    if (!is_null($username)) {
      $userid = AuthGetUserId($username);
    } else {
      $userid = $auth_userid;
    }

    if (!is_null($password)) {
      $pwhash = md5($password);
    } else {
      $pwhash = $auth_pwhash;
    }

    if (($userid > 0) &&
        ($result = MysqlQuery('SELECT COUNT(*) FROM Users WHERE id="' . $userid . '" AND password="' . $pwhash . '" AND type>"0"')) &&
        (mysql_result($result, 0, 0) == 1))
    {
      $auth_userid = $userid;
      $auth_pwhash = $pwhash;
      return true;
    } else {
      if (in_array(ErrGetLastError(), Array(E_NOERROR, E_AUTH_INVALID_USER_NAME, E_AUTH_UNKNOWN_USER_NAME)))
        ErrSetLastError(E_AUTH_NO_SUCH_USER_PWD);

      AuthLogout();
      return false;
    }
  }

  //
  // Invalidates the cached userid/hash, effectively logging
  //  the user out
  //
  function AuthLogout() {
    global $auth_userid, $auth_pwhash;

    unset($auth_userid, $auth_pwhash);

    return session_unregister('auth_userid') &&
           session_unregister('auth_pwhash');
  }

  //
  // Updates the current user's 'last login' time
  //
  function AuthUpdateLastLogin() {
    global $auth_userid, $auth_pwhash;
    return MysqlQuery('UPDATE Users SET time_lastlogin=NOW() WHERE id="' . $auth_userid . '" AND password="' . $auth_pwhash . '"');
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Account Management
  ////////////////////////////////////////////////////////////////////////////

  //
  // Updates the current user's alterable profile information
  //
  function AuthUpdateProfile($fullname, $email) {
    global $auth_userid, $auth_pwhash;
    ErrSetLastError();

    if (!AuthIsValidFullName($fullname)) {    // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_FULL_NAME);
      return false;
    }

    if (!AuthIsValidEmail($email)) {          // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_EMAIL);
      return false;
    }

    return MysqlQuery('UPDATE Users SET fullname="' . $fullname . '", email="' . $email . '" WHERE id="' . $auth_userid . '" AND password="' . $auth_pwhash . '"');
  }

  //
  // Changes the current user's password
  //
  function AuthChangePassword($oldpwd, $newpwd1, $newpwd2) {
    global $auth_userid, $auth_pwhash;
    ErrSetLastError();

    $oldhash = md5($oldpwd);

    if ($oldhash != $auth_pwhash) {
      ErrSetLastError(E_AUTH_NO_SUCH_USER_PWD);
      return false;
    }

    if (strcmp($newpwd1, $newpwd2) != 0) {
      ErrSetLastError(E_AUTH_PASSWORD_MISMATCH);
      return false;
    }

    if (strlen($newpwd1) < 4) {
      ErrSetLastError(E_AUTH_INVALID_PASSWORD);
      return false;
    }

    $newhash = md5($newpwd1);

    if (MysqlQuery('UPDATE Users SET password="' . $newhash . '" WHERE id="' . $auth_userid . '" AND password="' . $auth_pwhash . '"')) {
      $auth_pwhash = $newhash;
      return true;
    } else {
      return false;
    }
  }

  //
  // Creates a new user
  //
  function AuthCreateUser($username, $fullname, $email, $newpwd1, $newpwd2) {
    ErrSetLastError();

    $username = strtolower($username);
    $email = strtolower($email);

    if (!AuthIsValidUsername($username)) {   // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_USER_NAME);
      return false;
    }

    if (($result = MysqlQuery('SELECT COUNT(*) FROM Users WHERE name="' . $username . '"')) &&
        (mysql_result($result, 0, 0) > 0))
    {
      ErrSetLastError(E_AUTH_INUSE_NAME);
      return false;
    }

    if (!AuthIsValidFullName($fullname)) {    // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_FULL_NAME);
      return false;
    }

    if (!AuthIsValidEmail($email)) {          // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_EMAIL);
      return false;
    }

    if (strcmp($newpwd1, $newpwd2) != 0) {
      ErrSetLastError(E_AUTH_PASSWORD_MISMATCH);
      return false;
    }

    if (strlen($newpwd1) < 4) {
      ErrSetLastError(E_AUTH_INVALID_PASSWORD);
      return false;
    }

    AuthSRand();
    $salt = rand();

    return MysqlQuery('INSERT INTO Users (name,type,password,time_created,time_lastlogin,fullname,email,salt) VALUES ("' . $username . '","0","' . md5($newpwd1) . '",NOW(),NOW(),"' .$fullname . '","' . $email . '","' . $salt . '")');
  }

  //
  // Removes a user
  //
  function AuthDeleteUser($username, $password, $type) {
    ErrSetLastError();

    $username = strtolower($username);

    if (!AuthIsValidUsername($username)) {   // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_USER_NAME);
      return false;
    }

    return MysqlQuery('DELETE FROM Users WHERE name="' . $username . '" AND password="' . md5($password) . '" AND TYPE="' . $type . '"');
  }

  //
  // Compute an activation hash
  //
  function AuthGetVerificationHash($username) {
    $username = strtolower($username);

    if (($result = MysqlQuery('SELECT UCASE(MD5(CONCAT_WS(",",id,name,type,time_created,salt))) FROM Users WHERE name="' . $username . '"')) &&
        (list($hash) = mysql_fetch_row($result)))
    {
      return $hash;
    } else {
      return NULL;
    }
  }

  //
  // Activate a user account
  //
  function AuthActivateUser($username, $hash) {
    ErrSetLastError();

    $username = strtolower($username);

    if (!AuthIsValidUsername($username)) {   // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_USER_NAME);
      return false;
    }

    if (!AuthIsValidHash($hash)) {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }

    if (($result = MysqlQuery('SELECT id,name,type,time_created,salt FROM Users WHERE name="' . $username . '" AND UCASE(MD5(CONCAT_WS(",",id,name,type,time_created,salt)))=UCASE("' . $hash . '")')) &&
        (list($id,$name,$type,$time_created,$salt) = mysql_fetch_row($result)))
    {
      AuthSRand();
      $salt = rand();

      // Set type to active regular user and put a new salt in (for 'forgotten password' hash)
      return MysqlQuery('UPDATE Users SET type="1",salt="' . $salt . '" WHERE id="' . $id . '" AND name="' . $name . '"');
    } else {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }
  }

  //
  // Reset a user password
  //
  function AuthResetUserPassword($username, $hash, $newpwd1, $newpwd2) {
    ErrSetLastError();

    $username = strtolower($username);

    if (!AuthIsValidUsername($username)) {   // Check against SQL code injection
      ErrSetLastError(E_AUTH_INVALID_USER_NAME);
      return false;
    }

    if (!AuthIsValidHash($hash)) {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }

    if (strcmp($newpwd1, $newpwd2) != 0) {
      ErrSetLastError(E_AUTH_PASSWORD_MISMATCH);
      return false;
    }

    if (strlen($newpwd1) < 4) {
      ErrSetLastError(E_AUTH_INVALID_PASSWORD);
      return false;
    }

    if (($result = MysqlQuery('SELECT id,name,type,time_created,salt FROM Users WHERE name="' . $username . '" AND UCASE(MD5(CONCAT_WS(",",id,name,type,time_created,salt)))=UCASE("' . $hash . '")')) &&
        (list($id,$name,$type,$time_created,$salt) = mysql_fetch_row($result)))
    {
      AuthSRand();
      $salt = rand();

      // Reset the password and put a new salt in (for 'forgotten password' hash)
      return MysqlQuery('UPDATE Users SET password="' . md5($newpwd1) . '",salt="' . $salt . '" WHERE id="' . $id . '" AND name="' . $name . '"');
    } else {
      ErrSetLastError(E_AUTH_INVALID_HASH);
      return false;
    }
  }
?>
