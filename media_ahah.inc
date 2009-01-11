<?php

// $Id$

/**
 * This function builds the form elements for the file formater based
 * on the node type and field for settings and the file extension to get the formaters 
 * @param string $node_type
 *   the kind of node
 * @param string $field
 *   the machine name of the field being operated on
 * @param string $uri
 *   the uri to the item, or file://path if it is being uploaded
 * @param string $file_extension
 *   the kind of file that will be operated on
 */
function media_ahah_formatter_load($node_type, $field = '', $uri = 'file://', $file_extension = '') {
  global $user;

  // get the formaters for this node type
  $formatters = media_active_fields_for_node_type($node_type, 'formatter');
  // get the registrations
  $registrations = media_get_registered_modules($formatters[$field], 'formatter');
  // get all the formatting forms
  $formatter_options = array();
  $forms = array();
  foreach ($registrations as $id => $registration) {
    $formatter_options[$id] = $registration['name'];
    $function = $registration['callback'];
    $forms[$id] = $function($node_type, $field, $uri, $file_extension);    
  }

  return drupal_json(drupal_get_form('media_formater_ahah_form', $formatter_options, $forms)); //, $forms)); //, $formatter_options, $forms));
}


function media_formater_ahah_form($form_state, $options, $forms) {
  $form['formatter_select'] = array(    
    '#type' => 'select',
    '#title' => t('Select the formater to use'),
    '#options' => $options,
  );
  $form[] = $forms;
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'format'  
  );  
  return $form;
}