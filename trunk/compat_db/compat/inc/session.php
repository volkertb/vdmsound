<?php
  ////////////////////////////////////////////////////////////////////////////
  // One-time initial setup
  ////////////////////////////////////////////////////////////////////////////

  session_save_path('/home/groups/v/vd/vdmsound/sessions');

  ////////////////////////////////////////////////////////////////////////////
  // Methods
  ////////////////////////////////////////////////////////////////////////////

  function SessionStart($cache = ''/*'private'*/) {  // php bug with IE6
    session_cache_limiter($cache);
    return session_start();
  }

  function SessionDestroy() {
    return session_destroy();
  }
?>