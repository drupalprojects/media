<?php

// $Id$

/**
 * @info 
 * This file provides all the theming functions for the Media module
 */


/**
 * Display the media form when the tabs module is not present
 * @param array $tabs
 * @return string
 *   formated html
 */
function theme_media_no_tabs_display ($tabs) {  
  return "it would be a good idea to install tabs, huh.";
}


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