<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/auth.php');

  ////////////////////////////////////////////////////////////////////////////
  // 
  ////////////////////////////////////////////////////////////////////////////

  $html_uses_header = false;
  $html_uses_footer = false;
  $html_shows_modif = false;

  ////////////////////////////////////////////////////////////////////////////
  // Methods
  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlMakeURL($baseURL, $params = NULL) {
    global $SERVER_NAME, $SCRIPT_NAME;

    if (is_null($params))
      $params = Array();

    if (is_string($params)) {
      $pairs = explode('&', $params);
      $params = Array();

      foreach($pairs as $pair) {
        $keyvalue = explode('=', $pair);
        $params[$keyvalue[0]] = $keyvalue[1];
      }
    }

    if (eregi('^http://.+', $baseURL)) {
      $result = $baseURL;
    } else if (eregi('^/.+', $baseURL)) {
      $result = "http://" . $SERVER_NAME . $baseURL;
    } else {
      $result = "http://" . $SERVER_NAME . dirname($SCRIPT_NAME) . '/' . $baseURL;
    }

    if (strlen(SID) > 0) {
      $keyvalue = explode('=', SID);
      $params[$keyvalue[0]] = $keyvalue[1];
    }

    if (count($params) > 0) {
      $result .= '?';

      foreach ($params as $key => $value) {
        $result .= urlencode($key) . '=' . urlencode($value) . '&';
      }
    }

    return $result;
  }

  //
  //
  //
  function HtmlMakeLink($title, $baseURL, $params = NULL) {
    $result = '<a href="' . HtmlMakeURL($baseURL, $params);
    $result .= '">' . $title . '</a>';

    return $result;
  }

  //
  //
  //
  function HtmlBeginPage($title) {
    global $html_uses_header, $html_uses_footer, $html_shows_modif;
    $html_uses_header = false;
    $html_uses_footer = false;
    $html_shows_modif = false;

    echo('<html>');
    echo('<head>');
    echo('<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">');
    echo('<link rel="stylesheet" href="css/default.css" content="text/css">');
    echo('<title>' . $title . '</title>');
    echo('</head>');
    echo('<body style="color: #ffffff; background-color: #4f4f4f; margin: 0"><table border="0" cellspacing="0" cellpadding="0" width="100%" height="100%">');
  }

  //
  //
  //
  function HtmlBeginHeader($loggedIn) {
    global $html_uses_header;
    $html_uses_header = true;

    echo('<tr valign="top" height="0%"><td><div class="header">');
    echo('<table border="0" cellspacing="0" cellpadding="7" width="100%"><tr class="header"><td align="left">');
    HtmlSendUserInfo($loggedIn);
    echo('</td><td align="right">');
    HtmlSendUserMenu($loggedIn);
    echo('</td></tr></table>');
  }

  //
  //
  //
  function HtmlBeginBody() {
    global $html_uses_header;

    if ($html_uses_header) {
      echo('</div></td></tr>');
    }

    echo('<tr valign="middle" height="100%"><td>');
  }

  //
  //
  //
  function HtmlBeginFooter($showDate = true) {
    global $html_uses_footer, $html_shows_modif;
    $html_uses_footer = true;
    $html_shows_modif = $showDate;

    echo('</td></tr>');
    echo('<tr valign="bottom" height="0%"><td><div class="footer">');
  }

  //
  //
  //
  function HtmlEndPage() {
    global $html_uses_footer, $html_shows_modif;

    if ($html_shows_modif) {
      echo('<table border="0" cellspacing="0" cellpadding="7" width="100%"><tr style="color: #9f9f9f; font-size: 75%"><td align="center">');
      HtmlSendLastModified();
      echo('</td></tr></table>');
    }

    if ($html_uses_footer) {
      echo('</div>');
    }

    echo('</td></tr></table></body>');
    echo('</html>');
  }

  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlSendUserInfo($loggedin) {
    global $SCRIPT_NAME;

    if (!$loggedin) {
      echo('You are not logged in.');
    } else if ($userinfo = AuthGetUserInformation()) {
      echo('You are logged in as <cite>' . $userinfo['fullname'] . '</cite> (' . $userinfo['name'] . ').');
    } else {
      echo('Unable to determine login information: <cite>' . ErrFormatError(ErrGetLastError()) . ' (' . ErrGetLastError() . ')</cite>.');
    }
  }

  function HtmlSendUserMenu($loggedin) {
    global $SCRIPT_NAME;

    if ((strstr($SCRIPT_NAME, 'index.php') == ''))
      echo(HtmlMakeLink('Main', 'index.php') . '&nbsp;| ');

    if ((strstr($SCRIPT_NAME, 'profile.php') == '') && ($loggedin))
      echo(HtmlMakeLink('My Profile', 'profile.php') . '&nbsp;| ');

    if ($loggedin) {
      echo('<b>' . HtmlMakeLink('Logout', 'logout.php') . '</b>');
    } else {
      echo('<b>' . HtmlMakeLink('Login', 'login.php') . '</b>');
    }
  }

  //
  //
  //
  function HtmlSendLastModified() {
    echo('Last modified on ' . date('D M j G:i:s T Y', getlastmod()) . '.');
  }

  //
  //
  //
  function HtmlFormSendSID() {
    if (strlen(SID) > 0) {
      $keyvalue = explode('=', SID);
      echo('<input type="hidden" name="' . $keyvalue[0] . '" value="' . $keyvalue[1] . '">');
    }
  }

  //
  //
  //
  function HtmlHighlightError($text, $highlight = true) {
    $result = $text;

    if ($highlight) {
      $result = '<div class="hilite">' . $result . '</div>';
    }

    return $result;
  }
?>
