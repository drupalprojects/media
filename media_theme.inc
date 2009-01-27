<?php

// $Id$

/**
 * @info
 * This file provides all the theming functions for the Media module
 */

/**
 * theme function for displaying the media file browser
 * @param array $element
 *   form elements
 * @return string
 */
function theme_media_file_browser($element) {
  // add the CSS for our display
  $output = '<div class="media browser">'. $element .'</div>';



  return $output;
}