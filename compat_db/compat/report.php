<?php
  require_once('inc/error.php');
  require_once('inc/html.php');
  require_once('inc/apps.php');
  require_once('inc/session.php');
  require_once('inc/auth.php');

  SessionStart();

  if (!$loggedin = AuthLogin()) {
    header('Location: ' . HtmlMakeURL('login.php', Array('redirect' => $REQUEST_URI)));
    exit;
  }
?>

<?php HtmlBeginPage('Manage Compatibility Reports'); ?>
<?php HtmlBeginHeader($loggedin); ?>
<?php HtmlBeginBody(); ?>

<?php
  $userinfo = AuthGetUserInformation();

  $distribTypes = AppsGetDistributionTypes();
  $compatTypes  = AppsGetCompatTypes();
  $emuverTypes  = AppsGetEMUVerTypes();
  $osverTypes   = AppsGetOSVerTypes();

//$bla = AppsGetSimilar($title);
//echo('zzz');
//print_r($bla);
//echo('ZZZ');

  if ($action == 'new') {
    $reportid = NULL;

    if (isset($title, $distrib, $version, $os, $emu, $cvideo, $ckeyboard, $cmouse, $cjoystick, $cspeaker, $csb, $cadlib, $cmidi, $cgus, $cdisk, $cio, $ctimer, $comment)) {
      $title   = stripslashes($title);
      $version = stripslashes($version);
      $comment = stripslashes($comment);

      if (AppsAddReport($reportid, $title, $distrib, $version, $os, $emu, $cvideo, $ckeyboard, $cmouse, $cjoystick, $cspeaker, $csb, $cadlib, $cmidi, $cgus, $cdisk, $cio, $ctimer, $comment, /*false*/true)) {
        $style = 'success';
        $text  = 'Your report has been added to the database';

        $action = 'edit';    // go into 'edit' mode
      } else {
        $error = ErrGetLastError();

        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      }
    } else {
      $style = 'normal';
      $text  = 'Submit a new compatibility report';

      if (isset($appid)) {
        $appinfo = AppsGetByID($appid);
        $title   = $appinfo['title_text'];
        $distrib = $appinfo['distrib_id'];
        $version = $appinfo['appver_text'];
      }
    }
  } else if ($action == 'edit') {
    if (isset($reportid, $title, $distrib, $version, $os, $emu, $cvideo, $ckeyboard, $cmouse, $cjoystick, $cspeaker, $csb, $cadlib, $cmidi, $cgus, $cdisk, $cio, $ctimer, $comment)) {
      $title   = stripslashes($title);
      $version = stripslashes($version);
      $comment = stripslashes($comment);

      if (AppsAddReport($reportid, $title, $distrib, $version, $os, $emu, $cvideo, $ckeyboard, $cmouse, $cjoystick, $cspeaker, $csb, $cadlib, $cmidi, $cgus, $cdisk, $cio, $ctimer, $comment, /*false*/true)) {
        $style = 'success';
        $text  = 'Your report has been updated';
      } else {
        $error = ErrGetLastError();

        $style = 'error';
        $text  = ErrFormatError($error) . ' (' . $error . ')';
      }
    } else {
      $style = 'normal';
      $text  = 'Edit a compatibility report';

      if (isset($reportid)) {
        $myReports = AppsGetReports($reportid, NULL, NULL, true,
                                    APPS_GET_COMMENT | APPS_GET_AS_TEXT,
                                    APPS_GET_TITLE   | APPS_GET_APPVER  | APPS_GET_DISTRIB | APPS_GET_AS_TEXT   | APPS_GET_AS_ID,
                                    APPS_GET_OSVER   | APPS_GET_EMUVER  | APPS_GET_AS_ID,
                                    APPS_GET_COMPAT  | APPS_GET_AS_ID,
                                    NULL, false);

        if ($myReports && (count($myReports) > 0)) {
          $title     = $myReports[0]['title_text'];
          $distrib   = $myReports[0]['distrib_id'];
          $version   = $myReports[0]['appver_text'];
          $os        = $myReports[0]['osver_id'];
          $emu       = $myReports[0]['emuver_id'];

          $cvideo    = $myReports[0]['cvideo_id'];
          $ckeyboard = $myReports[0]['ckeyboard_id'];
          $cmouse    = $myReports[0]['cmouse_id'];
          $cjoystick = $myReports[0]['cjoystick_id'];
          $cspeaker  = $myReports[0]['cspeaker_id'];
          $csb       = $myReports[0]['csb_id'];
          $cadlib    = $myReports[0]['cadlib_id'];
          $cmidi     = $myReports[0]['cmidi_id'];
          $cgus      = $myReports[0]['cgus_id'];
          $cdisk     = $myReports[0]['cdisk_id'];
          $cio       = $myReports[0]['cio_id'];
          $ctimer    = $myReports[0]['ctimer_id'];

          $comment   = $myReports[0]['comment_text'];
        }
      } else {
        $action = 'new';
      }
    }
  } else if ($action == 'delete') {
    if (isset($reportid)) {
      if (isset($confirm)) {
        if (AppsDeleteReport($reportid)) {
          $text  = 'Your report has been deleted';
        } else {
          $error = ErrGetLastError();
          $text  = ErrFormatError($error) . ' (' . $error . ')';
        }
      }
    } else {
      $action = 'new';
    }
  } else {
    $action = 'new';
  }
?>

<?php
  if ($action == 'new') {
    $button = 'Create';
    $gui = 1;
  } else if ($action == 'edit') {
    $button = 'Update';
    $gui = 1;
  } else if ($action == 'delete') {
    if (!isset($confirm)) {
      $gui = 2;
    } else {
      $gui = 3;
    }
  }

  if ($gui == 1) {
    echo('<h1 class="' . $style . '">' . $text . '</h1>');

    echo('<form method="post" action="' . $SCRIPT_NAME . '" name="form_new"><table border="0" cellspacing="2" cellpadding="3" width="100%">');

    echo('<tr><td colspan="2" align="center" style="font: italic bold 125% serif; padding-top: 10px">General information</td></tr>');

    echo('<tr><td align="right" width="50%">Title:</td><td align="left" width="50%"><input class="flat1" type="text" size="24" name="title" value="' . $title . '" maxlength="255">' . '&nbsp;');
    echo('<script language="javascript"><!--' . "\n" . 'document.write("<input type=\\"button\\" class=\\"flat2\\" value=\\"Validate...\\" title=\\"Validate the application title against MobyGames\' database\\" onclick=\\"window.open(\'http://www.mobygames.com/search/quick?quickSearch=\' + escape(document.form_new.title.value), \'mobyframe\')\\">");' . "\n" . '//--></script>');
    echo('</td></tr>');

    echo('<tr><td align="right">Distribution:</td><td align="left"><select class="flat1" name="distrib">');
    HtmlSendOptions($distribTypes, 'id', 'description', $distrib);
    echo('</select></td></tr>');
    echo('<tr><td align="right" width="50%">Version (if any):</td><td align="left" width="50%"><input class="flat1" type="text" size="24" name="version" value="' . $version . '" maxlength="32"></td></tr>');

    echo('<tr><td colspan="2" align="center" style="font: italic bold 125% serif; padding-top: 10px">Test environment</td></tr>');

    echo('<tr><td align="right">Host operating system:</td><td align="left"><select class="flat1" name="os">');
    HtmlSendOptions($osverTypes, 'id', 'description', $os);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Emulation (if any):</td><td align="left"><select class="flat1" name="emu">');
    HtmlSendOptions($emuverTypes, 'id', 'description', $emu);
    echo('</select></td></tr>');

    echo('<tr><td colspan="2" align="center" style="font: italic bold 125% serif; padding-top: 10px">Test results</td></tr>');

    echo('<tr><td align="right">Video:</td><td align="left"><select class="flat1" name="cvideo">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cvideo);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Keyboard:</td><td align="left"><select class="flat1" name="ckeyboard">');
    HtmlSendOptions($compatTypes, 'id', 'description', $ckeyboard);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Mouse:</td><td align="left"><select class="flat1" name="cmouse">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cmouse);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Joystick:</td><td align="left"><select class="flat1" name="cjoystick">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cjoystick);
    echo('</select></td></tr>');
    echo('<tr><td align="right">PC Speaker:</td><td align="left"><select class="flat1" name="cspeaker">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cspeaker);
    echo('</select></td></tr>');
    echo('<tr><td align="right">SoundBlaster:</td><td align="left"><select class="flat1" name="csb">');
    HtmlSendOptions($compatTypes, 'id', 'description', $csb);
    echo('</select></td></tr>');
    echo('<tr><td align="right">FM / AdLib:</td><td align="left"><select class="flat1" name="cadlib">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cadlib);
    echo('</select></td></tr>');
    echo('<tr><td align="right">MPU401 / MIDI:</td><td align="left"><select class="flat1" name="cmidi">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cmidi);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Gravis UltraSound:</td><td align="left"><select class="flat1" name="cgus">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cgus);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Disk:</td><td align="left"><select class="flat1" name="cdisk">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cdisk);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Parallel / Serial:</td><td align="left"><select class="flat1" name="cio">');
    HtmlSendOptions($compatTypes, 'id', 'description', $cio);
    echo('</select></td></tr>');
    echo('<tr><td align="right">Timer:</td><td align="left"><select class="flat1" name="ctimer">');
    HtmlSendOptions($compatTypes, 'id', 'description', $ctimer);
    echo('</select></td></tr>');

    echo('<tr><td align="right">Additional comments:</td><td align="left"><textarea class="flat1" name="comment" rows="5" cols="40">' . htmlentities($comment) . '</textarea>');

    echo('<tr><td align="center" colspan="2"><input type="submit" class="flat2" value="' . $button . '">&nbsp;<input type="reset" class="flat2" value="Reset"></td></tr>');
    HtmlFormSendSID();
    echo('</table>');
    echo('<input type="hidden" name="action" value="' . $action . '">');
    echo('<input type="hidden" name="reportid" value="' . $reportid . '">');
    echo('</form>');
  } else if ($gui == 2) {
    $myReports = AppsGetReports($reportid, NULL, NULL, true,
                                0,
                                APPS_GET_TITLE | APPS_GET_APPVER  | APPS_GET_DISTRIB | APPS_GET_AS_TEXT,
                                APPS_GET_OSVER | APPS_GET_EMUVER  | APPS_GET_AS_TEXT,
                                0,
                                NULL, false);

    echo('<h1 class="normal">Removing a compatibility report</h1>');

    echo('<h2 class="normal">You are about to permanently delete the following report:</h2>');
    echo('<p align="center"><b>' . $myReports[0]['title_text'] . ' ' . $myReports[0]['version_text'] . ' (' . $myReports[0]['distrib_text'] . ')</b>, tested under <b>' . $myReports[0]['osver_text'] . '</b> using <b>' . $myReports[0]['emuver_text'] . '</b></p>');

    echo('<h2 class="normal">Do you want to proceed?</h2>');
    echo('<form method="get" action="' . $SCRIPT_NAME . '"><p align="center"><input type="hidden" name="action" value="delete"><input type="hidden" name="reportid" value="' . $reportid . '"><input type="hidden" name="confirm" value="yes"><input type="submit" class="flat2" value="Yes"></p></form>');
    echo('<form method="get" action="' . $SCRIPT_NAME . '"><p align="center"><input type="hidden" name="action" value="edit"><input type="hidden" name="reportid" value="' . $reportid . '"><input type="submit" class="flat2" value="No"></p></form>');
    echo('</p>');
  } else if ($gui == 3) {
    echo('<h2 class="normal">' . $text . '</h2>');
  }
?>

<?php HtmlBeginFooter(); ?>
<?php HtmlEndPage(); ?>
