<?php

// $Id$

/**
 * @info
 * This file provides all the theming functions for the Media module
 */

/**
 * Theme function for displaying the media file browser.
 * @param array $element
 *   The form element.
 * @return string
 */
function theme_media_file_browser($element) {
  // Add the CSS for our display.
  $output = '<div class="media browser">'. $element .'</div>';
  return $output;
}

/**
 * Theme function for displaying a media file list.
 * @param array $element
 *   The form element.
 * @return string
 */
function theme_media_file_list($element) {
  // Add the CSS for our display.
  return '<div class="media-file-list">'. theme('form_element', $element, $element['#children']) .'</div>';
}
