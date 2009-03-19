// $Id$

/**
 *  @file
 *  This file handles the JS for Media Module functions
 */


/**
 *  This handles the display, activation and
 *  hiding  of drawers on the media browser form.
 */
Drupal.behaviors.mediaDrawers = function (context) {

  // hide all the drawer display items on page load
  $('.media.browser .ui-tabs-panel .drawer.display:not(.mediaDrawers-processed)', context).addClass('mediaDrawers-processed').each(function() {
    $(this).hide();
  });

  // activate all the drawer data display that needs to be show when
  // the page is loaded
  $('.media.browser .ui-tabs-panel .drawer.display.active:not(.mediaDrawers-processed)', context).addClass('mediaDrawers-processed').each(function() {
    $(this).show();
  });

  // --------------------------------------
  // click actions

  // now we need to bind click functionality on drawers to display
  $('.media.browser .drawers .item-list ul li:not(.mediaDrawers-processed), .drawers li a:not(.mediaDrawers-processed)', context).addClass('mediaDrawers-processed').bind('click', function () {
    // get the href id that we want to display
    var display_id = $(this).attr('href');
    // this handles the LI click
    if (display_id == undefined) {
      var display_id = $(this).children('a').attr('href');
    }
    // we need to get the tab page that this drawer is in
    var parent = $(this).parents('.ui-tabs-panel').attr('id');
    // hide current active drawer display
    $('#'+parent+' .drawer.display.active').removeClass('active').hide();
    // set any drawers to not active
    $('#'+parent+' .drawers li.active').removeClass('active');
    // make this drawer active
    $(this).addClass('active');
    // make the requested drawer display active
    $(display_id).addClass('active').show();
    });

};

/**
 *  We need to hide any form elements that were replaced by the media browser
 *  form, activate the add button, and hide the browser.
 */
Drupal.behaviors.mediaBrowserHide = function (context) {
  $('.media.browser.wrapper:not(.mediaBrowserHide-processed)', context).addClass('mediaBrowserHide-processed').hide();
  $('.media.browser.activation:not(.mediaBrowserHide-processed)', context).addClass('mediaBrowserHide-processed').each(function () {
    $(this).next('.media.browser').hide();
    $(this).click(function () {
      $(this).next('.media.browser').slideDown('slow');
      $(this).slideUp();
    });
  });
};

/**
 *  Generate a MD5 hash of the file being uploaded
 */
Drupal.behaviors.mediaGenerateMD5 = function (context) {
  // Get the value from the file field.
  $('#edit-field-file-media-media-tabs-tab-Addfiles-media-upload-resource-Newfile-resource-form-media-upload-upload:not(.mediaGenerateMD5-processed)', context).addClass('mediaGenerateMD5-processed').change(function () {
    // Add the MD5 hash from the file name to the upload URL.
    Drupal.settings['ahah']['edit-attach']['url'] += '/'+$.md5($(this).val());
    // @TODO: Now add the MD5 value to the meta data form.
    alert(Drupal.settings['ahah']['edit-attach']['url']);
  });
};
