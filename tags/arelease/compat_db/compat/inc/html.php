<?php
  ////////////////////////////////////////////////////////////////////////////
  // Includes
  ////////////////////////////////////////////////////////////////////////////

  require_once('inc/auth.php');

  ////////////////////////////////////////////////////////////////////////////
  // One-time initial setup
  ////////////////////////////////////////////////////////////////////////////

  $html_body_bgcolor    = '#4f4f4f';
  $html_body_fgcolor    = '#ffffff';
  $html_body_linkcolor  = '#4fffff';
  $html_body_vlinkcolor = '#009f9f';
  $html_body_alinkcolor = '#7fff7f';

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
    global $html_body_bgcolor, $html_body_fgcolor, $html_body_linkcolor, $html_body_vlinkcolor, $html_body_alinkcolor;

    echo('<html>');
    echo('<head>');
    echo('<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">');
    echo('<title>' . $title . '</title>');
    echo('</head>');
    echo('<body bgcolor="' . $html_body_bgcolor . '" text="' . $html_body_fgcolor . '" link="' . $html_body_linkcolor . '" vlink="' . $html_body_vlinkcolor . '" alink="' . $html_body_alinkcolor . '">');
    echo('<table border="0" cellspacing="0" cellpadding="0" width="100%" height="100%">');
  }

  //
  //
  //
  function HtmlBeginHeader() {
    echo('<tr height="1" valign="top"><td>');
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
    echo('<tr height="1" valign="bottom"><td>');
  }

  //
  //
  //
  function HtmlEndPage() {
    echo('</td></tr></table></body>');
    echo('</body>');
    echo('</html>');
  }

  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlSendUserInfo($loggedin) {
    global $SCRIPT_NAME;
    echo('<table border="0" cellspacing="0" cellpadding="0" width="100%"><tr><td align="left"><font size="2">');

    if (!$loggedin) {
      echo('You are not logged in.');
    } else if ($userinfo = AuthGetUserInformation()) {
      echo('You are logged in as <i>' . $userinfo['fullname'] . '</i> (' . $userinfo['name'] . ').');
    } else {
      echo('Unable to determine login information: <i>' . ErrFormatError(ErrGetLastError()) . ' (' . ErrGetLastError() . ')</i>.');
    }

    echo('</font></td><td align="right"><font size="2">');

    if ((strstr($SCRIPT_NAME, 'index.php') == ''))
      echo(HtmlMakeLink('Main', 'index.php') . ' | ');

    if ((strstr($SCRIPT_NAME, 'profile.php') == '') && ($loggedin))
      echo(HtmlMakeLink('My Profile', 'profile.php') . ' | ');

    if ($loggedin) {
      echo('<b>' . HtmlMakeLink('Logout', 'logout.php') . '</b>');
    } else {
      echo('<b>' . HtmlMakeLink('Login', 'login.php') . '</b>');
    }

    echo('</font></td></tr></table>');
  }

  //
  //
  //
  function HtmlSendLastModified() {
    echo('<center><font size="2">Last modified on ' . date('D M j G:i:s T Y', getlastmod()) . '</font></center>');
  }

  //
  //
  //
  function HtmlSendTitlesHeader() {
    echo('<table ');
  }

  //
  //
  //
  function HtmlHighlightError($text, $highlight = true) {
    $result = $text;

    if ($highlight) {
      $result = '<b><font color="#ff7f7f">' . $result . '</font></b>';
    }

    return $result;
  }
?>
