<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/auth.php');

  ////////////////////////////////////////////////////////////////////////////
  // Methods
  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlMakeLink($title, $baseURL, $params = '') {
    $result = '<a href="' . $baseURL;

    if (strlen(SID) > 0) {
      $result .= '?' . SID;

      if (strlen($params) > 0) {
        $result .= '&' . $params;
      }
    } else {
      if (strlen($params) > 0) {
        $result .= '?' . $params;
      }
    }

    $result .= '">' . $title . '</a>';

    return $result;
  }

  //
  //
  //
  function HtmlBeginPage($title) {
    echo('<html>');
    echo('<head>');
    echo('<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">');
    echo('<link rel="stylesheet" href="css/default.css" content="text/css">');
    echo('<title>' . $title . '</title>');
    echo('</head>');
    echo('<body><table border="0" cellspacing="0" cellpadding="7" width="100%" height="100%">');
  }

  //
  //
  //
  function HtmlBeginHeader() {
    echo('<tr height="1" valign="top"><td class="header">');
  }

  //
  //
  //
  function HtmlBeginBody() {
    echo('</td></tr>');
    echo('<tr valign="center"><td>');
  }

  //
  //
  //
  function HtmlBeginFooter() {
    echo('</td></tr>');
    echo('<tr height="1" valign="bottom"><td class="footer">');
  }

  //
  //
  //
  function HtmlEndPage() {
    echo('</td></tr></table></body>');
    echo('</html>');
  }

  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlSendUserInfo($loggedin) {
    global $SCRIPT_NAME;

    echo('<div style="float: left">');

    if (!$loggedin) {
      echo('You are not logged in.');
    } else if ($userinfo = AuthGetUserInformation()) {
      echo('You are logged in as <i>' . $userinfo['fullname'] . '</i> (' . $userinfo['name'] . ').');
    } else {
      echo('Unable to determine login information: <i>' . ErrFormatError(ErrGetLastError()) . ' (' . ErrGetLastError() . ')</i>.');
    }

    echo('</div><div style="float: right">');

    if ((strstr($SCRIPT_NAME, 'index.php') == ''))
      echo(HtmlMakeLink('Main', 'index.php') . ' | ');

    if ((strstr($SCRIPT_NAME, 'profile.php') == '') && ($loggedin))
      echo(HtmlMakeLink('My Profile', 'profile.php') . ' | ');

    if ($loggedin) {
      echo('<b>' . HtmlMakeLink('Logout', 'logout.php') . '</b>');
    } else {
      echo('<b>' . HtmlMakeLink('Login', 'login.php') . '</b>');
    }

    echo('</div>');
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
  function HtmlHighlightError($text, $highlight = true) {
    $result = $text;

    if ($highlight) {
      $result = '<div class="hilite">' . $result . '</div>';
    }

    return $result;
  }
?>
