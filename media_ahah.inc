<?php

// $Id$

/**
 * This function builds the form elements for the file formater based
 * on the node type and field for settings and the file extension to get the formaters
 * @param string $registration_id
 *   the id of the registration creating this formatter
 * @param string $node_type
 *   the kind of node
 * @param string $field
 *   the machine name of the field being operated on
 * @param string $uri
 *   the uri to the item, or file://path if it is being uploaded
 * @param string $file_extension
 *   the kind of file that will be operated on
 */
function media_ahah_formatter_load($registration_id, $node_type, $field) {
  // Get the cache form data
  $form = form_get_cache($_POST['form_build_id'], &$cached_form_state);
  //

  //return drupal_json(''

  //eturn drupal_json('<pre>'. print_r($_POST, true) .'</pre><pre>'.print_r($form, true) .'</pre>');

//   media_ahah_debug($_POST);
//
//   exit();


  global $user;


  // get the file creator for this item
  $file_creator = media_get_registered_modules(array($registration_id));

  // get the formaters for this node type
  $formatters = media_active_fields_for_node_type($node_type, 'formatter');
  // get the registrations
  //print_r( media_get_registered_modules());
  $registrations = media_get_registered_modules($formatters[$field]);

  // remove any non-applying registrations
  $registrations = media_get_applicable_formatters($registrations, $file_extension);

  // get all the formatting forms
  $formatter_options = array();
  $forms = array();
  foreach ($registrations as $id => $registration) {
    $formatter_options[$id] = $registration['name'];
    $function = $registration['callbacks']['form'];
    if (function_exists($function)) {
      $forms[$id] = $function($node_type, $field, $file_extension);
    }
  }

  media_ahah_debug($forms);

  return drupal_json(drupal_get_form('media_formater_ahah_form', $formatter_options, $forms)); //, $forms)); //, $formatter_options, $forms));
}


function media_ahah_debug($array) {

  drupal_json('<pre>'. print_r($array, true) .'</pre>');
  exit();

}


/**
 * AHAH form displayed under the file selector
 *
 * @param array $form_state
 * @param array $options
 * @param array $forms
 * @return array
 *   drupal form array
 */
function media_formater_ahah_form($form_state, $options, $forms) {
  $form['formatter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Format options'),
  );

  // if we only have one option, display that
  if (count($options) > 1) {
    $form['formatter']['formatter_select'] = array(
      '#type' => 'select',
      '#title' => t('Select the formater to use'),
      '#options' => $options,
    );
  }
  $form['formatter'][] = $forms;
  $form['formatter']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'format'
  );
  return $form;
}