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
  // Methods - Generic
  ////////////////////////////////////////////////////////////////////////////

  //
  // Makes an absolute URL from a page name and list of GET-method arguments,
  //  also including the session ID for cookieless sessions.
  //
  function HtmlMakeURL($baseURL, $params = NULL) {
    global $SERVER_NAME, $SCRIPT_NAME;

    if (is_null($params))
      $params = Array();

    if (is_string($params)) {
      $pairs = explode('&', $params);
      $params = Array();

      foreach($pairs as $pair) {
        $keyvalue = explode('=', $pair, 2);
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
      $url = parse_url($result);
      if (strcasecmp($url['host'], $SERVER_NAME) == 0) {
        $keyvalue = explode('=', SID);
        $params[$keyvalue[0]] = $keyvalue[1];
      }
    }

    if (count($params) > 0) {
      $tmp = Array();

      foreach ($params as $key => $value) {
        array_push($tmp, urlencode($key) . '=' . urlencode($value));
      }

      $result .= '?' . implode('&', $tmp);
    }

    return $result;
  }

  //
  // Breaks an URL into base URL and parameters
  //
  function HtmlUnmakeURL($url, &$baseURL, &$params) {
    $params = Array();

    $tmp = explode('?', $url, 2);
    $baseURL = $tmp[0];
    $pairs = explode('&', $tmp[1]);

    foreach ($pairs as $pair) {
      $keyvalue = explode('=', $pair, 2);
      $params[$keyvalue[0]] = $keyvalue[1];
    }
  }

  //
  // Makes an '<a href="...">...</a>' link using the HtmlMakeURL function
  //
  function HtmlMakeLink($title, $baseURL, $params = NULL, $target = NULL) {
    $result = '<a href="' . HtmlMakeURL($baseURL, $params) . '"';

    if (!is_null($target))
      $result .= ' target="' . $target . '"';

    return $result . '>' . $title . '</a>';
  }

  //
  // Makes an '<img src="...">
  //
  function HtmlMakeImage($url, $alt = NULL, $border = 0, $hspace = 0, $vspace = 0, $align = NULL, $style = NULL) {
    if (strlen($url) < 1)
      return NULL;

    $result = '<img src="' . $url . '"';

    if (!is_null($alt))
      $result .= ' alt="' . $alt . '"';

    if (!is_null($align))
      $result .= ' align="' . $align . '"';

    if (!is_null($style))
      $result .= ' style="' . $style . '"';

    return $result . ' border="' . $border . '" hspace="' . $hspace . '" vspace="' . $vspace . '">';
  }

  //
  // Makes an "n out of m" results control
  //
  function HtmlMakeResultsInfo($start, $limit, $total, $itemName, $url) {
    if (!($limit > 0))
      $limit = max(1, $total - $start);

    $retval = Array();

    if ($limit < $total) {
      array_push($retval, $itemName . ' ' . ($start + 1) . '-' . ($start + $limit) . ' of ' . $total);
    }

    HtmlUnmakeUrl($url, $baseURL, $params);

    if ($start > 0) {
      $i = max(0, $start - $limit);
      $n = $start - $i;
      array_push($retval, HtmlMakeLink('previous ' . $n, $baseURL, array_merge($params, Array('i' => $i, 'n' => $limit))));
    }

    if ($start + $limit < $total) {
      $i = min($total - 2, $start + $limit);
      $n = min($limit, $total - $i);
      array_push($retval, HtmlMakeLink('next ' . $n, $baseURL, array_merge($params, Array('i' => $i, 'n' => $limit))));
    }

    if (count($retval > 0)) {
      return implode('&nbsp;| ', $retval);
    } else {
      return NULL;
    }
  }

  //
  // Makes an absolute URL from a page name and list of GET-method arguments,
  //  also including the session ID for cookieless sessions.
  //
  function HtmlFormatRawText($rawText) {
    $text = htmlentities($rawText);
    $text = ereg_replace("[[:alpha:]]+://[^<>[:space:]]+[[:alnum:]/]", "<a href=\"\\0\">\\0</a>", $text);
    return nl2br($text);
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Page layout
  ////////////////////////////////////////////////////////////////////////////

  //
  // Page
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
  // Page
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
      echo('<table border="0" cellspacing="0" cellpadding="7" width="100%"><tr valign="middle" style="color: #9f9f9f; font-size: 75%">');
      echo('<td width="150" align="left">&nbsp;</td><td align="center">');
      HtmlSendLastModified();
      echo('</td><td width="150" align="right">' . HtmlMakeImage('pics/browser.gif', 'Designed for') . HtmlMakeLink(HtmlMakeImage('pics/iexplorer.gif', 'Internet Explorer', 0, 3, 0), 'http://www.microsoft.com/ie/') . HtmlMakeLink(HtmlMakeImage('pics/mozilla.gif', 'Mozilla'), 'http://www.mozilla.org/') . '</td>');
      echo('</tr></table>');
    }

    if ($html_uses_footer) {
      echo('</div>');
    }

    echo('</td></tr></table></body>');
    echo('</html>');
  }

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Page layout (specialized)
  ////////////////////////////////////////////////////////////////////////////

  //
  //
  //
  function HtmlSendUserInfo($loggedIn) {
    global $SCRIPT_NAME;

    if (!$loggedIn) {
      echo('You are not logged in.');
    } else if ($userinfo = AuthGetUserInformation()) {
      echo('You are logged in as <cite>' . $userinfo['fullname'] . '</cite> (' . $userinfo['name'] . ').');
    } else {
      echo('Unable to determine login information: <cite>' . ErrFormatError(ErrGetLastError()) . ' (' . ErrGetLastError() . ')</cite>.');
    }
  }

  function HtmlSendUserMenu($loggedIn) {
    global $SCRIPT_NAME;

    if ($loggedIn)
      echo(HtmlMakeLink('Submit report', 'report.php', Array('action' => 'new')) . '&nbsp;< ');

    if ((strstr($SCRIPT_NAME, 'index.php') == ''))
      echo(HtmlMakeLink('Main', 'index.php') . '&nbsp;| ');

    if ((strstr($SCRIPT_NAME, 'profile.php') == '') && ($loggedIn))
      echo(HtmlMakeLink('My Profile', 'profile.php') . '&nbsp;| ');

    if ($loggedIn) {
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

  ////////////////////////////////////////////////////////////////////////////
  // Methods - Data presentation
  ////////////////////////////////////////////////////////////////////////////

  function HtmlSendNewsList($newsList, $footer = NULL) {
    echo('<table border="0" cellspacing="0" cellpadding="5" width="100%">');

    foreach ($newsList as $newsItem) {
      echo('<tr class="opaque1"><td class="opaque1_bevel">');
      echo('<div style="font: 60%">' . date('D M j Y', strtotime($newsItem['updated'])) . '</div> ');
      echo('<div style="font: serif; padding-left: 10px"><b>' . $newsItem['title'] . '</b></div> ');
      echo('<div style="font: 75% sans-serif; padding-left: 10px; color: #bfbfbf">' . HtmlFormatRawText($newsItem['comment']) . '</div>');

      echo('</td></tr><tr><td height="2"></td></tr>');
    }

    if (!is_null($footer)) {
      echo('<tr><td align="right">' . $footer . '</td></tr>');
    }

    echo('</table>');
  }

  function HtmlSendAppsList($appsList, $footer = NULL, $url = NULL, $sortkey = NULL, $sortasc = false) {
    echo('<table border="0" cellspacing="0" cellpadding="5" width="100%">');

    $numCols = 6;

    echo('<tr>' . __HtmlMakeSortKeyDropdown($url, Array('title_text' => 'application title', 'appver_text' => 'application version', 'distrib_text' => 'distribution media', 'reports_num' => 'number of reports', 'updated' => 'last update time'), $sortkey, $sortasc, '<th class="opaque1_bevel" colspan="' . $numCols . '" align="left"><div style="font: message-box">%go!Sort% applications by %sortkey% %sortasc%</div></th>') . '</tr>');

    echo('<tr>' .
      '<th align="left">Title' . __HtmlMakeSortKeyInline($url, 'title_text', $sortkey, $sortasc) . '</th>' .
      '<th align="center">Compatibility' . __HtmlMakeSortKeyInline($url, 'reports_num', $sortkey, $sortasc) . '</th>' .
      '<th align="center">Latest report' . __HtmlMakeSortKeyInline($url, 'updated', $sortkey, $sortasc) . '</th>' .
      '<th></th><th align="center">Description</th><th align="center">Download</th></tr>');

    foreach ($appsList as $appItem) {
      echo('<tr class="opaque1">');
      echo('<td class="opaque1_bevel" align="left"><b>' . $appItem['title_text'] . '</b> ' . $appItem['appver_text'] . ' <i><small>(' . $appItem['distrib_text'] . ')</small></i></td>');
      echo('<td class="opaque1_bevel" align="center">' . HtmlMakeLink('<b>' . $appItem['reports_num'] . ' report(s)</b>', 'list.php', Array('appid' => $appItem['app_id'])) . '</td>');
      echo('<td class="opaque1_bevel" align="center">' . date('Y/m/d', strtotime($appItem['updated'])) . '</td>');
      echo('<td class="opaque1_bevel opaque1_tearoff_v" width="10">&nbsp;</td>');
      echo('<td class="opaque1_bevel" align="center">' . HtmlMakeLink('view specs', 'http://www.mobygames.com/search/quick', Array('quickSearch' => $appItem['title_text']), '_blank') . '</td>');
      echo('<td class="opaque1_bevel" align="center">' . HtmlMakeLink('search 1', 'http://olesgames.virtualave.net/search2/search.cgi', Array('game' => $appItem['title_text']), '_blank') . ' / ' . HtmlMakeLink('2', 'http://www.freeoldies.com/search.php3', Array('keyword' => $appItem['title_text'])) . '</td>');
      echo('</tr>');
    }

    if (!is_null($footer)) {
      echo('<tr><td colspan="' . $numCols . '" align="right">' . $footer . '</td></tr>');
    }

    echo('</table>');
  }

  function __HtmlSendChecklistItem($label, $name, $reportItem) {
    $caps = $reportItem['emucaps_text'];

    if ((strlen($caps) == 0) || (stristr($caps, $name)) || ($reportItem['c' . $name . '_id'] > 30)) {
      echo('<tr valign="middle"><td nowrap align="right">' . $label . ':</td><td>' . HtmlMakeImage($reportItem['c' . $name . '_icon'], '', 0, 2, 0) . '</td><td nowrap align="left">' . $reportItem['c' . $name . '_text'] . '</td></tr>');
    }
  }

  function __HtmlMakeSortKeyInline($url, $key, $sortkey, $sortasc) {
    if (is_null($url) || is_null($sortkey))
      return '';

    if ($key == $sortkey) {
      $pic = $sortasc ? 'pics/sort_asc.gif' : 'pics/sort_desc.gif';
      $alt = $sortasc ? 'Sorted (ascending)' : 'Sorted (descending)';
      $asc = !$sortasc;
    } else {
      $pic = 'pics/sort.gif';
      $alt = 'Not sorted';
      $asc = true;
    }

    HtmlUnmakeUrl($url, $baseURL, $params);

    return '&nbsp;' . HtmlMakeLink(HtmlMakeImage($pic, $alt), $baseURL, array_merge($params, Array('sortkey' => $key, 'sortasc' => $asc)));
  }

  function __HtmlMakeSortKeyDropdown($url, $keys, $sortkey, $sortasc, $template) {
    if (is_null($url) || is_null($sortkey))
      return '';

    HtmlUnmakeUrl($url, $baseURL, $params);

    $form_params = '';
    foreach ($params as $key => $value) {
      if (($key != 'sortkey') && ($key != 'sortasc')) {
        $form_params .= '<input type="hidden" name="' . $key . '" value="' . $value . '">';
      }
    }

    $form_sortkey = '<select class="flat3" name="sortkey">';
    foreach ($keys as $key => $label) {
      $form_sortkey .= '<option value="' . $key . '"';
      if ($key == $sortkey) $form_sortkey .= ' SELECTED';
      $form_sortkey .= '>' . $label . '</option>';
    }
    $form_sortkey .= '</select>';
    $form_sortasc = '<label accesskey="a"><input type="radio" name="sortasc" value="1"' . ((!is_null($sortasc) && $sortasc) ? ' CHECKED' : '') . '> ascending</label>&nbsp;&nbsp;<label accesskey="a"><input type="radio" name="sortasc" value="0"' . ((!is_null($sortasc) && !$sortasc) ? ' CHECKED' : '') . '> descending</label>';
    $form_go = '<input type="submit" class="flat3" value="\\1">';

    $template = str_replace('%sortkey%', $form_sortkey, $template);
    $template = str_replace('%sortasc%', $form_sortasc, $template);
    $template = ereg_replace('%go!([^%]+)%', $form_go, $template);

    $retVal = '<form method="get" action="' . $baseURL . '">' . $form_params . $template . '</form>';

    return $retVal;
  }

  function HtmlSendReportList($reportList, $loggedIn, $briefFmt=true, $footer = NULL, $url = NULL, $sortkey = NULL, $sortasc = false) {
    echo('<table border="0" cellspacing="0" cellpadding="5" width="100%">');

    $lastAppID = NULL;

    if ($briefFmt) {
      $numCols = 2;
    } else {
      $numCols = 4;
    }

    echo('<tr>' . __HtmlMakeSortKeyDropdown($url, Array('updated' => 'last update time', 'user_text' => 'report contributor', 'title_text' => 'application title', 'appver_text' => 'application version', 'distrib_text' => 'distribution media', 'osver_text' => 'operating system', 'emuver_text' => 'emulator'), $sortkey, $sortasc, '<th class="opaque1_bevel" colspan="' . $numCols . '" align="left"><div style="font: message-box">%go!Sort% reports by %sortkey% %sortasc%</div></th>') . '</tr>');

    if ($loggedIn) {
      $userinfo = AuthGetUserInformation();
    }

    foreach ($reportList as $reportItem) {
      if ($briefFmt) {
        echo('<tr class="opaque1" valign="middle"><td class="opaque1_bevel">');
        echo('<div style="font: 60%">' . date('D M j Y', strtotime($reportItem['updated'])) . ' &nbsp; (contributed by <i>' . $reportItem['user_text'] . '</i>)</div>');
        echo('<div style="font: serif; padding-left: 10px"><b>' . $reportItem['title_text'] . '</b> ' . $reportItem['appver_text'] . ' <i><small>(' . $reportItem['distrib_text'] . ')</small></i></div>');
        echo('<div style="font: 75% sans-serif; padding-left: 10px; color: #bfbfbf">Tested under ' . $reportItem['osver_text'] . ' using ' . $reportItem['emuver_text'] . '</div>');

        echo('</td><td class="opaque1_bevel" align="right"><div class="compact">');
        echo(HtmlMakeLink('View', 'list.php', Array('reportid' => $reportItem['report_id'])));

        if ($loggedIn && ($reportItem['user_id'] == AuthGetUserId())) {
          echo('<br>' . HtmlMakeLink('Edit', 'report.php', Array('action' => 'edit', 'reportid' => $reportItem['report_id'])));
          echo('<br>' . HtmlMakeLink('Delete', 'report.php', Array('action' => 'delete', 'reportid' => $reportItem['report_id'])));
        }

        echo('</div></td></tr><tr><td colspan="2" height="2"></td></tr>');
      } else {
        $thisAppID = $reportItem['app_id'];

        if (is_null($thisAppID) || ($lastAppID != $thisAppID)) {
          echo('<tr><td colspan="3">');
          echo('<div style="font: 150% serif"><b>' . $reportItem['title_text'] . '</b> ' . $reportItem['appver_text'] . '</div>');

          echo('<table border="0" cellspacing="0" cellpadding="0" width="100%" style="padding-top: 5px; padding-bottom: 5px"><tr valign="middle" style="font-family: sans-serif">');
          echo('<td align="left" width="32">' . HtmlMakeImage($reportItem['distrib_icon'], NULL, 0, 8, 0) . '</td>');
          echo('<td align="left"><div style="font-size: 75%; color: #bfbfbf"><i>' . $reportItem['distrib_text'] . '</i></div></td>');

          if ($loggedIn) {
            echo('<td align="right">' . HtmlMakeLink('Add report', 'report.php', Array('action' => 'new', 'appid' => $reportItem['app_id'])) . '</td>');
          }

          echo('</tr></table></td></tr>');

          $lastAppID = $thisAppID;
        }

        echo('<tr class="opaque2"><td colspan="3" class="opaque2_bevel">');
        echo('<div class="compact">' . date('D M j Y', strtotime($reportItem['updated'])) . ' &nbsp; (contributed by <i>' . $reportItem['user_text'] . '</i>)<br>');
        echo(HtmlMakeImage($reportItem['emuver_icon'], $reportItem['emuver_text'], 0, 0, 0, "right") . HtmlMakeImage($reportItem['osver_icon'], $reportItem['osver_text'], 0, 5, 0, "right"));

        if ($loggedIn && ($reportItem['user_id'] == AuthGetUserId())) {
          echo('<div style="float: right">');
          echo(HtmlMakeLink('Edit', 'report.php', Array('action' => 'edit', 'reportid' => $reportItem['report_id'])) . '&nbsp;| ');
          echo(HtmlMakeLink('Delete', 'report.php', Array('action' => 'delete', 'reportid' => $reportItem['report_id'])) . '&nbsp;| ');
          echo('</div>');
        }

        echo('Tested under <b>' . $reportItem['osver_text'] . '</b> using <b>' . $reportItem['emuver_text'] . '</b></div>');
        echo('</td></tr>');

        echo('<tr class="opaque1" valign="top"><td style="padding-left: 15px"><table border="0" cellspacing="0" cellpadding="2" class="compact">');

        echo('<tr><td style="padding-bottom: 10px" colspan="3"><b>Compatibility checklist:</b></td></tr>');
        __HtmlSendChecklistItem('Video',             'video',    $reportItem);
        __HtmlSendChecklistItem('Keyboard',          'keyboard', $reportItem);
        __HtmlSendChecklistItem('Mouse',             'mouse',    $reportItem);
        __HtmlSendChecklistItem('Joystick',          'joystick', $reportItem);
        __HtmlSendChecklistItem('PC Speaker',        'speaker',  $reportItem);
        __HtmlSendChecklistItem('SoundBlaster',      'sb',       $reportItem);
        __HtmlSendChecklistItem('FM / AdLib',        'adlib',    $reportItem);
        __HtmlSendChecklistItem('MPU401 / MIDI',     'midi',     $reportItem);
        __HtmlSendChecklistItem('Gravis UltraSound', 'gus',      $reportItem);
        __HtmlSendChecklistItem('Disk',              'disk',     $reportItem);
        __HtmlSendChecklistItem('Parallel / Serial', 'io',       $reportItem);
        __HtmlSendChecklistItem('Timer',             'timer',    $reportItem);
        echo('</table></td><td width="20" class="opaque1_tearoff_v">&nbsp;</td><td width="100%"><table border="0" cellspacing="0" cellpadding="2" class="compact">');

        echo('<tr><td style="padding-bottom: 10px"><b>User comments:</b></td></tr>');
        echo('<tr><td class="opaque2" style="padding: 10px">' . HtmlFormatRawText($reportItem['comment_text']) . '</td></tr>');
        echo('</table></td></tr>');

        echo('<tr><td colspan="3" height="10"></td></tr>');
      }
    }

    if (!is_null($footer)) {
      echo('<tr><td colspan="' . $numCols . '" align="right">' . $footer . '</td></tr>');
    }

    echo('</table>');
  }

  function HtmlSendOptions($options, $id_name, $text_name, $id_sel = NULL) {
    foreach($options as $option) {
      $id_this = $option[$id_name];
      echo('<option value="' . $option[$id_name] . '"');
      if ($id_this == $id_sel) echo(' SELECTED');
      echo('>' . $option[$text_name] . '</option>');
    }
  }
?>
